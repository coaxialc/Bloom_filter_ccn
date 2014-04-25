/*
 * BootstrappingHelper.h
 *
 *  Created on: Oct 8, 2013
 *      Author: Coaxial
 */

#ifndef BOOTSTRAPPINGHELPER_H_
#define BOOTSTRAPPINGHELPER_H_

#include "ns3/mod-module.h"
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "ns3/CcnModule.h"
#include "ns3/Initializer.h"

using std::string;
using std::vector;
using std::set;

namespace ns3
{
class Text;
class Initializer;
class CCN_Name;

class BootstrappingHelper : public Object
{
	public:

	Ptr<Parser> p;
	vector < Ptr < CcnModule > > module;
	Ptr<CCN_Name> name2;
	Ptr<Initializer> i;
	Ptr<NetDevice> ndfinder(Ptr<Node> n1,Ptr<Node> n2);
	vector< Ptr < Object > > vec3;
	set <uint32_t> select(set <uint32_t>,unsigned gs);

	uint32_t selectOwner(set <uint32_t>,set <uint32_t>);
	void PITCheck(int ,int ,set<uint32_t>,Graph,uint32_t);
	unsigned gs;
	uint32_t interestsNum;
	uint32_t seed;
	string seedString;
	string output;
	void specificData();
	std::string filename;

	int length,switchh,d;

	BootstrappingHelper(string filename,string output,int gsize,uint32_t interestsNum,int switchh,uint32_t seed);
	~BootstrappingHelper();
	void parseTopology();

	void startExperiment();
	map <uint32_t , uint32_t> nodeToModule;

};
}


#endif
