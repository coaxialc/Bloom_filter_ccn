#include "ns3/Initializer.h"
#include <sstream>
#include <vector>

#include "ns3/CcnModule.h"

using std::stringstream;
using std::vector;
using std::queue;

namespace ns3
{

class CcnModule;

		Initializer::Initializer(vector < Ptr < CcnModule > >* module,Ptr<Parser> parser,int dataOwner,uint32_t dataNum,map <uint32_t , uint32_t> nodeToModule)
		{
			this->module=*module;
			this->parser=parser;
			this->dataOwner=dataOwner;
			this->dataNum=dataNum;
			this->nodeToModule=nodeToModule;

			for(uint32_t i=0;i<module->size();i++)
			{
				visited[module->at(i)->getNodeId()]=false;
			}
		}

		Initializer::~Initializer()
		{
			//delete connection;
		}

		void Initializer::doesEveryModuleHaveANode()
		{
			for(uint32_t i=0;i<module.size();i++)
			{
				if(module.at(i)->getNode()==0)
				{
					std::cout<<"Module "<<i<<" does not have a node."<<std::endl;
				}
			}
		}

		void Initializer::initializeFIBs()
		{
			queue<Ptr<CcnModule> > q = queue<Ptr<CcnModule> >();//queue construction
			Ptr<CcnModule> ccnm = module.at(nodeToModule.find(dataOwner)->second );
			q.push(ccnm);
			visited[ccnm->getNodeId()] = true;

			while(q.size()!=0)
			{
				Ptr<CcnModule> handle = q.front();
				q.pop();

				map<Ptr<NetDevice>, Ptr<CcnModule> > neighbors = handle->getNeighborModules();
				map<Ptr<NetDevice>, Ptr<CcnModule> >::iterator niter;
				for(niter=neighbors.begin(); niter!=neighbors.end(); niter++){
					Ptr<CcnModule> c = niter->second;
					if (visited[c->getNodeId()]){
						continue;
					}
					
					visited[c->getNodeId()] = true;

					for(uint32_t k=1;k<=this->dataNum;k++)
					{
						vector <string> nameVector;
						nameVector.push_back("domain1");
						nameVector.push_back("domain2");
						nameVector.push_back("domain3");
						stringstream sstream;
						sstream << k;
						nameVector.push_back(sstream.str());
						Ptr<CCN_Name> name=CreateObject<CCN_Name>(nameVector);
					//	std::cout << "Installing FIB entry on " << c->getNodeId() << std::endl;
						c->getFIB()->put(name,ndfinder(handle->getNode(),c->getNode()));
					}
					q.push(c);
				}
			}
			
			std::cout << "BFS visited " << visited.size() << " nodes\n";
		}

		Ptr<CcnModule> Initializer::firstUnvisitedChild(Ptr<CcnModule> ccn)
		{
			map<Ptr<NetDevice>, Ptr<CcnModule> >::iterator it;
			for(it = ccn->getNeighborModules().begin(); it != ccn->getNeighborModules().end(); it++){
				Ptr<CcnModule> neighborModule = it->second;
				
				if (!visited[neighborModule->getNodeId()]){
					return neighborModule;
				}
			}
			
			/*
			for(unsigned i=0;i<ccn->getNode()->GetNDevices();i++)
			{
				if(!(this->visited.find(ccn->getNeighborModules().find(ccn->getNode()->GetDevice(i))->second)->second))
				{
						return ccn->getNeighborModules().find(ccn->getNode()->GetDevice(i))->second;
				}				
				
				 * if(!ccn->getNeighborModules().find(ccn->getNode()->GetDevice(i))->second->getNode()==0)//checking if its a real module (explained in BootstrappingHelper.cc)
				{
					
				}
			}
			*/

			return 0;
		}


		Ptr<NetDevice> Initializer::ndfinder(Ptr<Node> n1,Ptr<Node> n2)//epistrefei to net device tou deksiou me to opoio o deksis syndeetai ston aristero
		{
		//	std::cout<<"ndfinder: "<<n1->GetId()<<" "<<n2->GetId()<<std::endl;

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

			return 0;
		}
}
