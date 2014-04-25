
#ifndef PARSER_H_
#define PARSER_H_


//#include "ns3/network-module.h"
//#include "ns3/internet-module.h"
//#include "ns3/point-to-point-module.h"
//#include "ns3/applications-module.h"
#include <vector>
#include <string>
#include <map>
#include <set>
//#include <boost/graph/adjacency_list.hpp>
#include "ns3/core-module.h"
#include "ns3/node.h"
#include "ns3/Graph.h"

using std::vector;
using std::map;
using std::set;
using std::string;

/*typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> graph;
typedef boost::graph_traits<graph>::vertex_descriptor vertex;
typedef boost::graph_traits<graph>::edge_descriptor edge;*/

namespace  ns3 {

class Parser : public Object
{
	public:
	map<uint32_t, set<uint32_t> > matrix_map;
	map<uint32_t, Ptr<Node> > idToNode;
	map<uint32_t, uint32_t > nodeToId;

	Parser();
	~Parser();
	virtual void DoDispose(void);

	Ptr<Node> getNodeById(uint32_t);
	uint32_t findId(Ptr<Node>);

	vector<Ptr<Node> > getNeighbors(uint32_t);

	void parse(string &);
	//graph topology;
	//graph getGraph();
	Graph topology;
	Graph getGraph();
};

}  // namespace  ns3
#endif /* PARSER_H_ */
