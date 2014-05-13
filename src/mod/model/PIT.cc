#include <map>
#include "PIT.h"

using std::pair;

namespace ns3 {

int PIT::COUNT_PITS = 0;

PIT::PIT() {
	COUNT_PITS++;
	table = map<Ptr<CCN_Name>, Ptr<PTuple> >();
}

PIT::~PIT() {
	table.clear();
}

void PIT::DoDispose(void) {
	table.clear();

	COUNT_PITS--;

	if(COUNT_PITS==0)
	{
		//std::cout<<"PITS over."<<std::endl;
	}
}

void PIT::update(Ptr<CCN_Name> key, Ptr<PTuple> re) {
	table[key] = re;
}

void PIT::erase(Ptr<CCN_Name> key) {
	table.erase(key);
}

void PIT::eraseArgs(Ptr<CCN_Name> key,Ptr<LocalApp> app)
{
	Ptr<PTuple> p=check(key);
	if(p!=0 && p->getLocalApp()==app)
	{
		table.erase(key);
	}
}

void PIT::eraseArgs(Ptr<CCN_Name> key,Ptr<NetDevice> d)
{
	Ptr<PTuple> p=check(key);
	if(p!=0 && p->getDevice()==d)
	{
		table.erase(key);
	}
}

Ptr<PTuple> PIT::check(Ptr<CCN_Name> key)
{
	map<Ptr<CCN_Name>, Ptr<PTuple> >::iterator find = table.find(key);

	if (find != table.end())
	{
		return find->second;
	}
	else
	{
		return 0;
	}
}

Ptr<PTuple> PIT::checkArgs(Ptr<CCN_Name> name,  Ptr<NetDevice>  device)
{
	map<Ptr<CCN_Name>, Ptr<PTuple> >::iterator find = table.find(name);

	if (find != table.end())
	{
		if(find->second->getDevice()==device)
		{
			return find->second;
		}

		return 0;
	}
	else
	{
		return 0;
	}
}

Ptr<PTuple> PIT::checkArgs(Ptr<CCN_Name> name, Ptr<LocalApp>  app)
{
	map<Ptr<CCN_Name>, Ptr<PTuple> >::iterator find = table.find(name);

	if (find != table.end())
	{
		if(find->second->getLocalApp()==app)
		{
			return find->second;
		}

		return 0;
	}
	else
	{
		return 0;
	}
}

uint32_t PIT::getSize() {
	return table.size();
}

/*bool PIT::addRecord(Ptr<CCN_Name> key, Ptr<Bloomfilter> f, uint32_t ttl){
	Ptr<PTuple> tuple = check(key);
	if (tuple == 0){
		tuple = CreateObject<PTuple>(f, ttl);
		update(key, tuple);
		return true;
	}else{
		tuple->getBF()->OR(f);
		if (ttl > tuple->getTTL()){
			tuple->setTTL(ttl);
		}
		return false;
	}
}

bool PIT::addRecordArgs(Ptr<CCN_Name> name,Ptr<LocalApp>  app,  Ptr<Bloomfilter> f,  uint32_t ttl)
{
	Ptr<PTuple> tuple = check(name);
	if (tuple == 0){
		tuple = CreateObject<PTuple>(f, ttl);
		update(name, tuple);
		return true;
	}else{
		tuple->getBF()->OR(f);
		if (ttl > tuple->getTTL()){
			tuple->setTTL(ttl);
		}
		return false;
	}
}

bool PIT::addRecordArgs(Ptr<CCN_Name> name,Ptr<NetDevice>  device, Ptr<Bloomfilter> f,  uint32_t ttl)
{
	Ptr<PTuple> tuple = check(name);
	if (tuple == 0){
		tuple = CreateObject<PTuple>(f, ttl);
		update(name, tuple);
		return true;
	}else{
		tuple->getBF()->OR(f);
		if (ttl > tuple->getTTL()){
			tuple->setTTL(ttl);
		}
		return false;
	}
}*/

}
