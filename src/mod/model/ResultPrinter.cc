#include "ResultPrinter.h"

#include <iostream>
#include <sstream>

using std::stringstream;
using std::ofstream;

using namespace ns3;

ResultPrinter::ResultPrinter(vector<Ptr<CcnModule> >& module, uint32_t gs, uint32_t exp, Ptr<Sender> s, vector<Ptr<Receiver> > receiver,std::string output)
{
	this->module = module;

	//std::cout << "-----------------Interest and data count-----------------"	<< std::endl;

	//std::cout << CcnModule::RX_INTERESTS << " interests appeared." << std::endl	<< CcnModule::RX_DATA << " data appeared." << std::endl;

	//std::cout << "-----------------Interest and data count-----------------"	<< std::endl;

	for (uint32_t i = 0; i < module.size(); i++)
	{
		stringstream st;
		st << gs;

		stringstream st2;
		st2 << exp;

		ofstream file;

		file.open(	(output+"BF"+"/data_stats/gs-" + st.str() + "-experiment-" + st2.str()	+ ".txt").c_str(), std::ios::app);
		file << "router " << module.at(i)->getNodeId() << " data_count " << module.at(i)->getTXData() << std::endl;
		file.close();
	}

	/*for (uint32_t i = 0; i < receiver.size(); i++)
	{

		//std::cout<<"receiver found"<<std::endl;
		if (receiver.at(i)->getReturned() != receiver.at(i)->getAskedFor())
		{
			std::cout << "A Receiver did not get everything it asked for!"	<< std::endl;
		}
		std::cout << "Receiver got " << receiver.at(i)->getReturned() << ".Asked for " << receiver.at(i)->getAskedFor() << std::endl;

	}*/

	//std::cout << "Sender got " << s->getInterests() << " interests." << std::endl;
}

ResultPrinter::~ResultPrinter() {

}

void ResultPrinter::DoDispose(void) {

}
