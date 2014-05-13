#include "CcnModule.h"
#include "experiment_globals.h"

#include <iostream>
#include <sstream>
#include <string>
#include <bitset>
#include <list>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::bitset;
using std::list;

namespace ns3 {

uint32_t CcnModule::RX_INTERESTS = 0;
uint32_t CcnModule::RX_DATA = 0;
int CcnModule::COUNT = 0;

Time CcnModule::ONE_NS = NanoSeconds(1);

CcnModule::CcnModule(Ptr<Node> node, int switchh) {
	COUNT++;
	//std::cout<< "Installing CCN module on ns3::node " << node->GetId() << std::endl;
	nodePtr = node;	

	this->switchh = switchh; //if zero ,hop counters start randomly ,otherwise they all start at max ,which is d

	thePIT = CreateObject<PIT>();
	FIB = CreateObject<Trie>();
	p_RX_Data = 0;
	addresses = map<Ptr<NetDevice>, Address>();

	for (uint32_t i = 0; i < nodePtr->GetNDevices(); i++) {

		Ptr<NetDevice> device = nodePtr->GetDevice(i);
		device->SetReceiveCallback(
				MakeCallback(&CcnModule::handlePacket, this));

		Ptr<Channel> channel = device->GetChannel();
		Address adr;
		if (device == channel->GetDevice(0)) {
			adr = channel->GetDevice(1)->GetAddress();
		} else {
			adr = channel->GetDevice(0)->GetAddress();
		}
		addresses[device] = adr;
	}

	deviceToLid = map<Ptr<NetDevice>, Ptr<Bloomfilter> >();
}

CcnModule::~CcnModule() {
	thePIT = 0;
	FIB = 0;
	nodePtr = 0;
	deviceToLid.clear();
	addresses.clear();
}

void CcnModule::reset()
{
	neighborModules.clear();
	deviceToLid.clear();
	addresses.clear();
}

void CcnModule::DoDispose(void) {
	thePIT = 0;
	FIB = 0;
	nodePtr = 0;
	deviceToLid.clear();
	addresses.clear();
	COUNT--;
	//std::cout<<"COUNT: "<<COUNT<<std::endl;
}

void CcnModule::reInit() {
	thePIT = 0;
	thePIT = CreateObject<PIT>();
	p_RX_Data = 0;

	FIB = 0;
	FIB = CreateObject<Trie>();
}

void CcnModule::sendInterest(Ptr<CCN_Name> name, Ptr<LocalApp> localApp){
	Simulator::Schedule(CcnModule::ONE_NS, &CcnModule::doSendInterest, this, name, localApp);
}

void CcnModule::sendData(Ptr<CCN_Name> name, uint8_t *buff, uint32_t bufflen, Ptr<LocalApp> app){ //std::cout<<"Sender is "<<this->getNodeId()<<""<<std::endl;
	uint8_t *newBuff = (uint8_t*)malloc(bufflen);
	memcpy(newBuff, buff, bufflen);
	Simulator::Schedule(CcnModule::ONE_NS, &CcnModule::doSendData, this, name, newBuff, bufflen,app);
}


void CcnModule::sendThroughDevice(Ptr<const Packet> p, Ptr<NetDevice> nd) {
	uint8_t* b = new uint8_t[p->GetSize()];
	p->CopyData(b, p->GetSize());
	Ptr<Packet> p2 = Create<Packet>(b, p->GetSize());
	delete [] b;
	
	if(nd==0)
	{
		std::cout << "device null" << std::endl;
	}

	bool sent = nd->Send(p2, addresses[nd], 0x88DD);

	if (!sent) {
		std::cout << "bytes dropped" << std::endl;
		std::cout << "packets dropped" << std::endl;
	}	
}

bool CcnModule::handlePacket(Ptr<NetDevice> nd, Ptr<const Packet> p, uint16_t a,
		const Address& ad) {
	uint8_t type = extract_packet_type(p);
	if (type == CCN_Packets::INTEREST) {
		RX_INTERESTS++;
		handleIncomingInterest(p, nd);
	} else if (type == CCN_Packets::DATA) {
		RX_DATA++;
		p_RX_Data++;
		handleIncomingData(p, nd);
	}

	return true;
}

uint8_t CcnModule::extract_packet_type(Ptr<const Packet> p) {
	uint8_t b2 = 0;
	p->CopyData(&b2, sizeof(uint8_t));
	return b2;
}

void CcnModule::handleIncomingInterest(Ptr<const Packet> p, Ptr<NetDevice> nd) {

	//say it when you get an interest
	//std::cout<<"Node "<<this->nodePtr->GetId()<<" just got the interest."<<std::endl;

	Ptr<CCN_Interest> interest = CCN_Interest::deserializeFromPacket(p->Copy());

	interest->decreaseHopCounter();
	Ptr<Bloomfilter> outgoingLID = this->deviceToLid.find(nd)->second;
	interest->getBloomfilter()->OR(outgoingLID); //update BF in interest packet
	
	Ptr<PTuple> pt = this->thePIT->check(interest->getName());
	
							/*	if(this->getNodeId()==10)
							{
									std::cout<<"ttl in packet is "<<interest->getHopCounter()<<""<<std::endl;
									//std::cout<<"ttl  "<<<<""<<std::endl;
								}*/

	if (pt != 0){
		pt->getBF()->OR(interest->getBloomfilter());
		
		//uint8_t new_ttl = ExperimentGlobals::D - interest->getInitialHopCounter();
		uint8_t new_ttl = interest->getInitialHopCounter() - interest->getHopCounter();
		if (new_ttl > pt->getTTL()) {
			pt->setTTL(new_ttl);
		}
		return;
	}	

	//PIT tuple not found
	Ptr<TrieNode> tn = this->FIB->longestPrefixMatch(interest->getName());	
	if(tn->hasLocalApps())
	{
		Ptr<LocalApp> publisher = tn->getLocalApps().at(0);

		//save to PIT first, next hop is publisher
		//uint8_t ttl2=ExperimentGlobals::D-interest->getInitialHopCounter();
		uint8_t ttl2=interest->getInitialHopCounter() - interest->getHopCounter();
		Ptr<PTuple> publisherTuple=CreateObject<PTuple>(interest->getBloomfilter(),ttl2,publisher);
		//Ptr<PTuple> publisherTuple=CreateObject<PTuple>(interest->getBloomfilter(),ExperimentGlobals::D-interest->getHopCounter(),publisher);

		this->thePIT->update(interest->getName(),publisherTuple);//std::cout<<"Inserting in PIT ,Bf: "<<publisherTuple->getBF()->toString()<<std::endl;

		if(this->nodePtr->GetId()==18)
		{
			//std::cout<<"Inserting in PIT ,hc: "<<(int)publisherTuple->getTTL()<<std::endl;
		}

		publisher->deliverInterest(interest->getName());
		return;
	}

	NS_ASSERT_MSG(tn->hasDevices(),
			"router " + nodePtr->GetId() << "does not know how to forward " << interest->getName()->toString());
			
	//interest will go to the first netdevice
	Ptr<NetDevice> outport = tn->getDevices().at(0);

	if (interest->getHopCounter() == 0) { //must store in PIT
		//uint8_t new_ttl = ExperimentGlobals::D - interest->getInitialHopCounter();
		uint8_t new_ttl = interest->getInitialHopCounter();

		Ptr<PTuple> ptuple = CreateObject<PTuple>(interest->getBloomfilter(),
				new_ttl, outport);

		thePIT->update(interest->getName(), ptuple); //std::cout<<"Inserting in PIT ,Bf: "<<ptuple->getBF()->toString()<<std::endl;
		if(this->nodePtr->GetId()==18)
				{

					//std::cout<<"Inserting in PIT ,hc: "<<(int)ptuple->getTTL()<<std::endl;
				}
		//reset interest status
		interest->setTtl(ExperimentGlobals::D);
		//interest->setinitialHopCounter(0);
		interest->setinitialHopCounter(ExperimentGlobals::D);
		Ptr<Bloomfilter> newBF = CreateObject<Bloomfilter>(Bloomfilters::BF_LENGTH);
		interest->setBloomfilter(newBF);
	}

	Ptr<Packet> packet = interest->serializeToPacket();
	sendThroughDevice(packet, outport);

}

void CcnModule::handleIncomingData(Ptr<const Packet> p, Ptr<NetDevice> nd) {	
	Ptr<CCN_Data> data = CCN_Data::deserializeFromPacket(p->Copy()); //std::cout<<"HC "<<(int)data->getHopCounter()<<std::endl;
	NS_ASSERT_MSG(data->getHopCounter() > 0, "hop counter is " << (int)data->getHopCounter());
	data->decreaseHopCounter();
	
	//std::cout<<"Node "<<this->nodePtr->GetId()<<" just got the data, " << data->getName()->toString() << " ";
	//std::cout<<"TTL "<< (int)data->getHopCounter() << std::endl;

	//always check PIT
	Ptr<PTuple> pt = this->thePIT->checkArgs(data->getName(), nd);

	//watch this carefully, we 'll discuss this closely on Tuesday
	if (pt != 0) { //interest has been tracked at PIT
		thePIT->eraseArgs(data->getName(), nd);

		//give data to any local app
		vector<Ptr<LocalApp> >& localApps = pt->getLocalApps();
		for (uint32_t i = 0; i < localApps.size(); i++) { //std::cout<<"delivering"<<std::endl;
			localApps[i]->deliverData(data->getName(), data->getData(),
					data->getLength());
		}

		Ptr<Bloomfilter> tmpBF = pt->getBF(); //std::cout<<"BF "<<tmpBF->toString()<<std::endl;
		Ptr<Bloomfilter> bf = CreateObject<Bloomfilter>(tmpBF->getLength(),
				tmpBF->getBuffer());
		uint32_t ttl = pt->getTTL(); //std::cout<<"TTL "<<ttl<<std::endl;

		if (data->getHopCounter() == 0) {
			//replace bf and ttl in packet
			data->setBloomfilter(bf);
			data->setTtl(ttl);
		} else {
			//THIS IS TRICKY,
			//Data packet does not terminate here, either send two packets OR
			//aggregate the two BFs and transmit once
			//Must see what causes less false positives
			data->getBloomfilter()->OR(bf);
			if (data->getHopCounter() < ttl) {
				data->setTtl(ttl);
			//	std::cout << "Data relay, node " << this->nodePtr->GetId() << ", data name" << data->getName()->toString() << " , ttl "<< ttl << std::endl;
			}
		}
	}

	if (data->getHopCounter() > 0) {
		bfForward(data->getBloomfilter(), data, nd);
	}
}

uint32_t CcnModule::bfForward(Ptr<Bloomfilter> bf, Ptr<CCN_Data> data,
		Ptr<NetDevice> excluded) {
	uint32_t fwded = 0;
	for (unsigned i = 0; i < this->nodePtr->GetNDevices(); i++) {
		Ptr<NetDevice> device = this->nodePtr->GetDevice(i);
		if (device == excluded) {
			continue;
		}

		Ptr<Bloomfilter> lid = deviceToLid[device];
		if (bf->contains(lid)) {
			fwded++;
			Ptr<Packet> p = data->serializeToPacket();
			Address address = addresses[device];
			bool sent = device->Send(p, address, 0x88DD);

			if (!sent) {
				std::cout << "bytes dropped" << std::endl;
				std::cout << "packets dropped" << std::endl;
			}
		}
	}
	return fwded;
}

void CcnModule::doSendInterest(Ptr<CCN_Name> name, Ptr<LocalApp> localApp) {
	Ptr<PTuple> pt = thePIT->checkArgs(name,localApp);
	if (pt != 0) {
		bool added = pt->addLocalApp(localApp);
		if (!added) {
			cout << "local app has already requested " << name->toString()
					<< endl;
		}
		return;
	}

	Ptr<Bloomfilter> bf = CreateObject<Bloomfilter>(Bloomfilters::BF_LENGTH);
	uint32_t initTTL = decideTtl();
	Ptr<CCN_Interest> interest = CreateObject<CCN_Interest>(name, initTTL,
			initTTL, bf);
	Ptr<TrieNode> fibLookup = this->FIB->longestPrefixMatch(name);
	if (fibLookup == 0) {
		stringstream sstr;
		sstr << "router " << nodePtr->GetId() << "cannot forward Interests for "
				<< name->toString();
		string mesg = sstr.str();
		NS_ASSERT_MSG(fibLookup != 0, mesg);
	}

	pt = CreateObject<PTuple>(bf, 0,fibLookup->getDevices().at(0));//an app asks us to send it ,so it must be forwarded to a device ,we use the first device
	pt->addLocalApp(localApp);
	//pt->setDevice(fibLookup->getDevices()->at(0));
	
	thePIT->update(name, pt);//mipos prepei na kanoume addrecord anti gia update?bebaia mallon mas to zitaei proti fora opote de tha yparxei idi eggrafi

	if (fibLookup->hasLocalApps()) {
		Ptr<LocalApp> publisher = fibLookup->getLocalApps().at(0);
		// pt = CreateObject<PTuple>(bf, 0, publisher);
		// pt->addLocalApp(localApp);
		// thePIT->updateArgs(name, pt);
		publisher->deliverInterest(name);
	} else if (fibLookup->hasDevices()) {
		Ptr<NetDevice> netdevice = fibLookup->getDevices().at(0);
		// pt = CreateObject<PTuple>(bf, 0, netdevice);
		// pt->addLocalApp(localApp);
		// thePIT->updateArgs(name, pt);
		Ptr<Packet> p = interest->serializeToPacket();

		sendThroughDevice(p, netdevice);
	} else {
		stringstream sstr;
		sstr << "router " << nodePtr->GetId() << "cannot forward Interests for "
				<< name->toString();
		string mesg = sstr.str();
		NS_ASSERT_MSG(fibLookup != 0, mesg);
		NS_ASSERT_MSG(false, mesg);
	}
}

int CcnModule::decideTtl() {
	if (switchh == 0) {
		uint32_t d=ExperimentGlobals::RANDOM_VAR->GetInteger(1,ExperimentGlobals::D);
		//std::cout<<"random ttl: "<<d<<std::endl;
		return d;
	} else {
		//otan theloyme panta megisto hc prepei par ola auta na kanoune ena axristo get gia
		//na min allaksei i akoloytheia ton random arithmon mas kai na synexisoun
		//na epilegontai oi idio komboi (ektos an de to theloyme auto)
		ExperimentGlobals::RANDOM_VAR->GetInteger(1,ExperimentGlobals::D);
		return ExperimentGlobals::D;
	}
}

Ptr<Trie> CcnModule::getFIB() {
	return FIB;
}

Ptr<Node> CcnModule::getNode() {
	return nodePtr;
}

map<Ptr<NetDevice>, Ptr<CcnModule> > CcnModule::getNeighborModules() {
	return neighborModules;
}

Ptr<PIT> CcnModule::getPIT() {
	return thePIT;
}

void CcnModule::doSendData(Ptr<CCN_Name> name, uint8_t* buff, uint32_t bufflen,Ptr<LocalApp> app) {
	if(this->getNodeId()==92)
	std::cout<<"Cheking in doSendData , "<<name->toString()<<" and "<<app<<std::endl;
	Ptr<PTuple> ptuple = thePIT->checkArgs(name,app);
	if (ptuple == 0) {
		stringstream sstr;
		sstr << "router " << nodePtr->GetId() << "has no pit record for "
				<< name->toString();
		string mesg = sstr.str();
		NS_ASSERT_MSG(ptuple != 0, mesg);
	}
	NS_ASSERT_MSG(ptuple->getTTL() > 0, "Data TTL in PIT is zero");

	uint32_t deliverd = 0;
	vector<Ptr<LocalApp> >::iterator iter;
	for (iter = ptuple->getLocalApps().begin();
			iter != ptuple->getLocalApps().end(); iter++) {
		deliverd++;
		(*iter)->deliverData(name, buff, bufflen);
	}

	Ptr<CCN_Data> data = CreateObject<CCN_Data>(ptuple->getBF(),
			ptuple->getTTL(), name, buff, bufflen);	
	uint32_t fwded = bfForward(data->getBloomfilter(), data, 0);
	NS_ASSERT_MSG(fwded > 0 || deliverd > 0, "data went nowhere");

	free(buff);
}

void CcnModule::installLIDs() {
	for (uint32_t i = 0; i < this->nodePtr->GetNDevices(); i++) {
		Ptr<NetDevice> device = this->nodePtr->GetDevice(i);
		std::stringstream s;
		s << device->GetAddress();
		std::string result1 = md5(s.str());
		std::string result2 = sha1(s.str());

		uint32_t integer_result1 = (bitset<32>(
				stringtobinarystring(result1).substr(96))).to_ulong(); //we only keep the last 32 bits
		uint32_t integer_result2 = (bitset<32>(
				stringtobinarystring(result2).substr(96))).to_ulong(); //we only keep the last 32 bits

		Ptr<Bloomfilter> lid = CreateObject<Bloomfilter>(
				Bloomfilters::BF_LENGTH);

		//Double hashing scheme
		for (uint32_t j = 0; j < Bloomfilters::K_HASHES; j++) {
			uint32_t index = (integer_result1 + j * j * integer_result2)
					% (Bloomfilters::BF_LENGTH*8);
			lid->setBit(index);
		}
		deviceToLid[device] = lid;
	}
}

string CcnModule::stringtobinarystring(std::string s) {
	string result = "";

	for (std::size_t i = 0; i < s.size(); ++i) {
		result = result + (bitset<8>(s.c_str()[i])).to_string();
	}

	return result;
}

bool operator<(const Ptr<NetDevice>& lhs, const Ptr<NetDevice>& rhs) {
	if(lhs==0)
	{
		std::cout<<"first was null"<<std::endl;
	}

	if(rhs==0)
	{
		std::cout<<"second was null"<<std::endl;
	}
	return lhs->GetAddress() < rhs->GetAddress();
}

uint32_t CcnModule::getNodeId()
{
	return nodePtr->GetId();
}

uint32_t CcnModule::getTXData()
{
	return p_RX_Data;
}

}
