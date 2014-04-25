#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mod-module.h"
#include "ns3/BootstrappingHelper.h"


using namespace ns3;

int main(int argc ,char *argv[])
{
	CommandLine c;
	c.Parse (argc, argv);

	ExperimentGlobals::RANDOM_VAR =CreateObject<UniformRandomVariable>();

	//Topology path
	//--------------------------------------------------
	//string topology = "/home/coaxial/Desktop/topologies/geant.txt";
	string topology = *(new std::string(argv[1]));
	//--------------------------------------------------

	//Output path
	//--------------------------------------------------
	//string output="/home/coaxial/Desktop/exp_output/";
	string output = *(new std::string(argv[2]));
	//--------------------------------------------------

	//Seed
	//--------------------------------------------------
	RngSeedManager::SetSeed (std::atoi(argv[3]));
	//RngSeedManager::SetSeed (81);
	//--------------------------------------------------

	//Group size
	//--------------------------------------------------
	//uint8_t gs=2;
	uint8_t gs=std::atoi(argv[4]);
	//--------------------------------------------------

	//Interest number
	//--------------------------------------------------
	//uint8_t interestNum=100;
	uint8_t interestNum=std::atoi(argv[5]);
	//--------------------------------------------------

	//BF size (in bytes)
	//--------------------------------------------------
	//Bloomfilters::BF_LENGTH=16;
	Bloomfilters::BF_LENGTH=std::atoi(argv[6]);
	//--------------------------------------------------

	//K value
	//--------------------------------------------------
	//Bloomfilters::K_HASHES=4;
	Bloomfilters::K_HASHES=std::atoi(argv[7]);
	//--------------------------------------------------

	//D value
	//--------------------------------------------------
	//ExperimentGlobals::D=8;
	ExperimentGlobals::D=std::atoi(argv[8]);
	//--------------------------------------------------

	//Switch to random D
	//--------------------------------------------------
	//uint8_t sw=0;
	uint8_t sw=std::atoi(argv[9]);
	//--------------------------------------------------

	std::cout<<"D: "<<ExperimentGlobals::D<<std::endl;

	Ptr<BootstrappingHelper> bh=CreateObject<BootstrappingHelper>(topology,output,gs,interestNum,sw,std::atoi(argv[3]));


	bh->parseTopology();

	bh->startExperiment();

	std::cout<<"Application ending."<<std::endl;
	Simulator::Destroy();
	return 0;
}
