#ifndef GRAPH_H_
#define GRAPH_H_

#include <map>
#include <set>
#include "ns3/core-module.h"
#include <vector>

using std::set;
using std::map;
using std::vector;

namespace ns3 {


class Graph: public Object {
public:

	Graph();
	~Graph();
	virtual void DoDispose(void);
	set<uint32_t> getNodesWithDegree(uint32_t);
	bool isItCoreNode(uint32_t);
	void setGraph(map<uint32_t, set<uint32_t> >);
	void Dijkstra(uint32_t);
	uint32_t closestUnvisited();
	void results();

private:
	map<uint32_t, set<uint32_t> > graph;
	vector<uint32_t> apostasi;
	vector<int32_t> progonos;
	vector<bool> remember;
	uint32_t source;

};
}


#endif
