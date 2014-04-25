#include "BootstrappingHelper.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

using std::ofstream;
using std::stringstream;
using std::vector;
using std::string;
using std::set;

using std::endl;

using namespace ns3;

BootstrappingHelper::BootstrappingHelper(string filename,string output,int gsize,uint32_t interestsNum,int switchh,uint32_t seed)
{
	std::cout<<"Constructing BootstrappingHelper"<<std::endl;
	this->seed=seed;
	this->filename=filename;
	this->interestsNum=interestsNum;
	this->output=output;
	vec3= std::vector< Ptr < Object > >();
	this->gs=gsize;
	this->switchh=switchh;
	nodeToModule=map<uint32_t, uint32_t >();//to find module index in the vector that is connected to specific node (by its id)
}

BootstrappingHelper::~BootstrappingHelper()
{
	std::cout<<"Destructing BootstrappingHelper"<<std::endl;
	p=0;
	vector<Ptr<CcnModule> >::iterator iter;
	for (iter=module.begin(); iter!=module.end(); iter++ )
	{
		(*(iter))->reset();
	}



	module.clear();
	i=0;
	vec3.clear();
}

void BootstrappingHelper::parseTopology()
{
	p=CreateObject<Parser>();
	p->parse(filename);
	module = vector < Ptr < CcnModule > >();

	map<uint32_t, Ptr<Node> >::iterator iter;

	for (iter=p->idToNode.begin(); iter!=p->idToNode.end(); iter++ ){//iterating using iterator ,not size and counters ,because in the middle there are numbers we do not want to use
		Ptr<CcnModule> m = CreateObject<CcnModule>(iter->second,this->switchh);
		nodeToModule[iter->first]=module.size();//if there are x already inside ,then the next will be the (x+1)th ,with index x
		module.push_back(m);
	}

	for(unsigned i=0;i<module.size();i++)//here we use the size and counter because we want to refer to vector indexes (module's)
	{
		module.at(i)->installLIDs();
	}

	map<uint32_t, Ptr<Node> >::iterator iter2;

	//here we set the neighbors of every CcnModule
	for (iter2=p->idToNode.begin(); iter2!=p->idToNode.end(); iter2++ )//here we want an iterator again ,because we must not try to refer to nodes that do not exist
	{
		if(iter2->first!=0)
		{
			for(unsigned j=0;j<p->getNeighbors(iter2->first).size();j++)//for every neighbor
				{
					Ptr<Node> n1=p->getNeighbors(iter2->first).at(j);//this is the neighbor

					uint32_t u1=p->nodeToId[n1->GetId()];//neighbors id (the id we use ,not the system's)

					Ptr<CcnModule> ccn_module=module.at(nodeToModule.find(u1)->second);//neighbor's module

					Ptr<Node> n2=iter2->second;//this node

					uint32_t u2=iter2->first;//this node's id

					Ptr<NetDevice> net_device = ndfinder(n1,n2);

					if(net_device==0)
					{
						std::cout<<"net_device about to be inserted in the map is null"<<std::endl;
					}

					if(ccn_module==0)
					{
						std::cout<<"ccn_module about to be inserted in the map is null"<<std::endl;
					}

					//std::cout<<"Setting neighbor of module "<<nodeToModule.find(u2)->second<<" to be "<<nodeToModule.find(u1)->second<<std::endl;
					module.at(nodeToModule.find(u2)->second)->setNeighbor(net_device, ccn_module);
				}
		}
	}

	std::cout<<"Module vector has size: "<<module.size()<<std::endl;
	//std::cout<<"According to the maps:"<<std::endl;
	
	for(uint32_t i=0;i<module.size();i++)
	{
		Ptr<CcnModule> from = module.at(i);
		map<Ptr<NetDevice>, Ptr<CcnModule> > neighbors = from->getNeighborModules();
		for(uint32_t j=0;j<from->getNode()->GetNDevices();j++)//yparxoune netdevices poy den einai syndedemenes poyhtena ,etsi fainetai apo edo ,opote prepei na prosexoume na min asxoloumaste mazi tous
		{	
			Ptr<NetDevice> dev = from->getNode()->GetDevice(j);
			if(neighbors.find(dev) != neighbors.end())
			{
			//std::cout<<"Node "<<from->getNode()->GetId()<<" is connected through device "<<dev->GetAddress()<<" to node "<<from->getNeighborModules().find(dev)->second->getNode()->GetId()<<std::endl;
			}

		}
	}
	
}

Ptr<NetDevice> BootstrappingHelper::ndfinder(Ptr<Node> n1,Ptr<Node> n2)//epistrefei to net device tou deksiou me to opoio o deksis syndeetai ston aristero
{
	//std::cout<<"Bootstrapping ndfinder"<<std::endl;
	if(n1==0) {std::cout<<"In ndfinder in initializer n1 is null"<<std::endl;}

	if(n2==0) {std::cout<<"In ndfinder in initializer n2 is null"<<std::endl;}

	for(unsigned i=0;i<n2->GetNDevices();i++)
	{
			if(n2->GetDevice(i)->GetChannel()->GetDevice(0)->GetNode()==n1)
			{
					return n2->GetDevice(i);
			}

			if(n2->GetDevice(i)->GetChannel()->GetDevice(1)->GetNode()==n1)
			{
					return n2->GetDevice(i);
			}
	}

	std::cout<<"Bootstrapping helper ndfinder returning 0"<<std::endl;
	return 0;
}

void BootstrappingHelper::startExperiment()
{
	std::cout<<"startExperiment"<<std::endl;

	Graph topology=p->getGraph();

	set<uint32_t> d1 = topology.getNodesWithDegree(1);

	std::cout<<"degree 1 exoun : "<<d1.size()<<std::endl;

	if(this->gs>d1.size()-1)
	{
		exit(EXIT_FAILURE);
	}

	{

		std::cout<<"******************************************************"<<std::endl;
		std::cout<<"                  Group size: "<<gs<<std::endl;
		std::cout<<"******************************************************"<<std::endl<<std::endl<<std::endl;

		for(unsigned c=0;c<20;c++)
		{
			ExperimentGlobals::RANDOM_VAR =CreateObject<UniformRandomVariable>();
			RngSeedManager::SetSeed (seed+c);

			stringstream st;
			st << seed+c;
			this->seedString=st.str();

			set <uint32_t> group=select(d1,gs);
			uint32_t dataOwner=selectOwner(d1,group);
			std::cout<<"Module vector size "<<module.size()<<std::endl;
			//set owner app and data object
			//----------------------------------------------------

			std::cout<<"Data owner is "<<dataOwner<<" --- ns3 id of node: "<<module.at( nodeToModule.find( dataOwner )->second)->getNodeId()<<std::endl;

			//bazoume sender sto module poy exei to kombo 106 ,oxi sto 106o module (to opoio den yparxei ypoxreotika)
			Ptr<Sender> sa1=CreateObject<Sender>( module.at( nodeToModule.find( dataOwner )->second )  , 60);



			char d2 []={'h','e','l','l','o'};

			vector < Ptr < CCN_Name > > nameVector;
			vector<string> nv;

			for(uint32_t i=1;i<=interestsNum;i++)
			{
				nv.clear();
				nv.push_back("domain1");
				nv.push_back("domain2");
				nv.push_back("domain3");

				stringstream sstream;
				sstream << i;

				nv.push_back(sstream.str());
				Ptr<CCN_Name> name=CreateObject<CCN_Name>(nv);
				nameVector.push_back(name);
			}

			for(uint32_t i=0;i<interestsNum;i++)
			{				
				Ptr<Packet> packet = Create<Packet>((uint8_t*)d2, (uint32_t)strlen(d2));
				sa1->insertData(nameVector.at(i), packet);

				const pair < ns3::Ptr< CCN_Name >, int > pa2 (nameVector.at(i),5);
			}



			for(uint32_t i=0;i<interestsNum;i++)
			{
				//std::cout<<"putting: "<<nameVector->at(i)->toString()<<std::endl;

				if(sa1->getLocalApp()==0)
				{
					std::cout<<"XONO NULL STON OWNER"<<std::endl;
				}

				module.at(nodeToModule.find( dataOwner )->second)->getFIB()->put(nameVector.at(i),sa1->getLocalApp());
			}

			//----------------------------------------------------


			//set group apps and request data
			//----------------------------------------------------
			vector < Ptr < Receiver > > vec;

			vector<string> name3;
			name3.push_back("domain1");
			name3.push_back("domain2");
			name3.push_back("domain3");

			Ptr<CCN_Name> name4=CreateObject<CCN_Name>(name3);

			for (set<uint32_t>::iterator it=group.begin(); it!=group.end(); ++it)
			{
				vec.push_back(CreateObject<Receiver>(module.at(nodeToModule.find(*it)->second)));
			}

			this->i=CreateObject<Initializer>(&module,p,dataOwner,interestsNum,nodeToModule);
			this->i->initializeFIBs();

			for(unsigned i=0;i<gs;i++)
			{
				vec.at(i)->SendInterest(name4,interestsNum);
			}
			//----------------------------------------------------

			//PIT check
			//----------------------------------------------------
			Time t=ns3::Seconds(30);
			Simulator::Schedule(t,&BootstrappingHelper::PITCheck,this,gs,c,group,topology,dataOwner);
			//----------------------------------------------------


			Simulator::Run();


			//print results and initialize structures again
			//----------------------------------------------------
			Ptr<ResultPrinter> rp=CreateObject<ResultPrinter>(module,gs,c,sa1,vec,output);

			//print to file
			//----------------------------------------------------
			ofstream file;
			string tempPath=output+"BF"+"/results.txt";
			file.open (tempPath.c_str(),std::ios::app);

			stringstream st2;
			st2 << ExperimentGlobals::D;

			file << "experiment "<<c<<" groupsize "<<gs<<" D "<<st2.str()<<" participants ["<<module.at(nodeToModule.find( dataOwner )->second)->getNodeId()<<",";  //prota emfanizetai i pigi

			for(unsigned i=0;i<gs;i++)
			{
				file<<vec.at(i)->getModule()->getNodeId();

				if(i!=gs-1)
				{
					file<<",";
				}
			}

			file<<"] interests "<<CcnModule::RX_INTERESTS<<" data "<<CcnModule::RX_DATA<<"\n";

			file.close();

			//----------------------------------------------------

			CcnModule::RX_INTERESTS=0;
			CcnModule::RX_DATA=0;

			for(unsigned i=0;i<module.size();i++)
			{
				module.at(i)->reInit();
			}

			//----------------------------------------------------

			Simulator::Stop();
		}
	}
}

set <uint32_t> BootstrappingHelper::select(set <uint32_t> s,unsigned gs)
{
	vector<uint32_t> v(s.begin(),s.end());
	set <uint32_t> group;

	for(unsigned i=0;i<gs;i++)
	{
		int pos=ExperimentGlobals::RANDOM_VAR->GetInteger(i+1,v.size()-1);

		std::swap(v[i],v[pos]);
	}

	for(unsigned i=0;i<gs;i++)
	{
		group.insert(v.at(i));
	}

	return group;
}

uint32_t BootstrappingHelper::selectOwner(set <uint32_t> d1,set <uint32_t> group)
{
	set <uint32_t> d;

	for (set<uint32_t>::iterator it=d1.begin(); it!=d1.end(); ++it)
	{
		if(group.find(*it)==group.end())
		{
			d.insert(*it);
		}
	}

	int pos=ExperimentGlobals::RANDOM_VAR->GetInteger(0,d.size()-1);

	set<uint32_t>::const_iterator it(d.begin());
    advance(it,pos);

	return *it;
}

void BootstrappingHelper::PITCheck(int gs,int exp,set<uint32_t> group,Graph topology,uint32_t owner)
{//sto router tis pigis tha mou bgalei allo ena entry

	string group_nodes="";

	for (set<uint32_t>::iterator it=group.begin(); it!=group.end(); ++it)
	{
		std::stringstream s;
		s << *it;

		group_nodes=group_nodes+","+s.str();
	}

	for(unsigned i=0;i<module.size();i++)
	{
		if(module.at(i)->getNodeId() != owner )
		{
			stringstream st;
			st << gs;

			stringstream st2;
			st2 << exp;

			stringstream st3;
			st3 << ExperimentGlobals::D;

			ofstream file;

			string secondPartPath="BF/pit_stats/gs-"+st.str()+"-experiment-"+st2.str()+"-D-"+st3.str()+"-group_nodes-"+group_nodes+"-seed-"+seedString+".txt";
			string tempPath=output+secondPartPath;
			const char* tempPath2=tempPath.c_str();

			file.open (tempPath2,std::ios::app);

			file<<"router "<<module.at(i)->getNodeId()<<" pit_entries "<<module.at(i)->getPIT()->getSize();

			if(topology.isItCoreNode(module.at(i)->getNodeId()))
			{
				file<<" core";
			}
			else
			{
				file<<" access";
			}

			file<<endl;

			file.close();
		}
	}
}
