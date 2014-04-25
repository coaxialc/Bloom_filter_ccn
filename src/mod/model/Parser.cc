#include "Parser.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "utils.h"

using std::ifstream;
using std::istringstream;
using std::stringstream;
using std::getline;

namespace ns3 {


Parser::Parser() {
	matrix_map = map<uint32_t, set<uint32_t> >();
	idToNode = map<uint32_t, Ptr<Node> >() ;
	nodeToId = map<uint32_t, uint32_t >();
}

Parser::~Parser() {
	//TODO implement correctly
}

void Parser::DoDispose(void){
	idToNode.clear();
}


Ptr<Node> Parser::getNodeById(uint32_t nodeid){
	return idToNode[nodeid];
}

uint32_t Parser::findId(Ptr<Node> node){
	return nodeToId[node->GetId()];
}

vector<Ptr<Node> > Parser::getNeighbors(uint32_t graphNodeId){
	set<uint32_t> neighbors = matrix_map[graphNodeId];
	vector<Ptr<Node> > v;

	//bool entered=false;

	set<uint32_t>::iterator iter;
	for (iter=neighbors.begin(); iter!=neighbors.end(); iter++ ){
		uint32_t n_id = *iter;
		Ptr<Node> nodePtr = idToNode[n_id];
		v.push_back(nodePtr);
		//entered=true;
	}

	/*if(!entered)
	{
		uint32_t n_id = *iter;
		Ptr<Node> nodePtr = idToNode[n_id];
		v.push_back(nodePtr);
	}*/

	return v;
}

//new one
void Parser::parse(string& filepath) {//	std::cout<<"PARSING"<<std::endl;
	matrix_map.clear();

	ifstream f;
	string line;
	f.open(filepath.c_str());

	if (f.is_open()) {
		while (getline(f, line)) {
			vector<string> tokens = Utils::split(line, ' ');
			string t = "";
			for(uint32_t i=0; i<tokens.size(); i++){
				string orgnl = tokens[i];
				Utils::replaceAll(orgnl, "<", "");
				tokens[i] = orgnl;
			}
			uint32_t sourceNode = atoi(tokens[0].c_str());
			set<uint32_t> neighbors;
			for(uint32_t i=1; i<tokens.size(); i++){
				uint32_t neighbor = atoi(tokens[i].c_str());
				matrix_map[sourceNode].insert(neighbor);
				matrix_map[neighbor].insert(sourceNode);
			}
		}
	}

	map<uint32_t, set<uint32_t> >::iterator iter;
	NodeContainer n;

	for (iter=matrix_map.begin(); iter!=matrix_map.end(); iter++ ){
		uint32_t node = iter->first;
		Ptr<Node> nodePtr = CreateObject<Node>();
		n.Add(nodePtr);
		idToNode[node] = nodePtr;
		nodeToId[nodePtr->GetId()] = node;
	}

	set<uint32_t> nodes;

	map<uint32_t, map<uint32_t, uint32_t> > alreadyConnected;
	map<uint32_t, map<uint32_t, uint32_t> > alreadyConnected2;
	for (iter=matrix_map.begin(); iter!=matrix_map.end(); iter++ ){
		Ptr<Node> sourceNode = getNodeById(iter->first);
		vector<Ptr<Node> > neighbors = getNeighbors(iter->first);
		vector<Ptr<Node> >::iterator neighborsIter;
		for (neighborsIter=neighbors.begin(); neighborsIter!=neighbors.end(); neighborsIter++ ){
			uint32_t sourceNodeGraphId = nodeToId[sourceNode->GetId()];
			uint32_t neighborNodeGraphId = nodeToId[(*neighborsIter)->GetId()];

			if (alreadyConnected[sourceNodeGraphId][neighborNodeGraphId] == 1){
				continue;
			}else{

			}

			NodeContainer n;
			n.Add(sourceNode);
			n.Add(*neighborsIter);

			PointToPointHelper pph;
			pph.SetQueue(string("ns3::DropTailQueue"),
					string("MaxPackets"),
					ns3::UintegerValue(10000), string("MaxBytes"),
					ns3::UintegerValue(10000));
			pph.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
			pph.SetChannelAttribute("Delay", StringValue("2ms"));
			NetDeviceContainer ndc = pph.Install(n);

			nodes.insert(sourceNodeGraphId);
			nodes.insert(neighborNodeGraphId);

			alreadyConnected[sourceNodeGraphId][neighborNodeGraphId] = 1;
			alreadyConnected[neighborNodeGraphId][sourceNodeGraphId] = 1;
		}
	}

	/*map<uint32_t, set<uint32_t> > topmap;

	for(uint32_t i=0;i<nodes.size();i++)
	{
		topmap.insert(i, {} );
	}

	for(uint32_t i=0;i<nodes.size();i++)
	{
		for(uint32_t j=0;j<nodes.size();j++)
		{
			if( alreadyConnected[i][j]==1 && alreadyConnected2[i][j]==0 )
			{
				topmap.find(i)->second.insert(j);

				alreadyConnected2[i][j] = 1;
				alreadyConnected2[j][i] = 1;
			}
		}
	}

	topology.setGraph(topmap);*/

//	std::cout<<"Parser: Graph size + 1: "<<matrix_map.size()+1<<std::endl;

	topology.setGraph(matrix_map);
	//topology.Dijkstra(3);
}

Graph Parser::getGraph() {
	return topology;
}

}
