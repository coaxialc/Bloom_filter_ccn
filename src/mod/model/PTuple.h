/*
 * PTuple.h
 *
 *  Created on: Nov 26, 2013
 *      Author: Coaxial
 */

#ifndef PTUPLE_H_
#define PTUPLE_H_

#include <vector>
#include "ns3/core-module.h"
#include "ns3/Bloomfilter.h"
#include "ns3/local_app.h"
#include <ns3/network-module.h>

using std::vector;

namespace ns3 {

class LocalApp;
class Bloomfilter;

class PTuple : public Object {
public:
	static int COUNT_TUPLES;
	//PTuple(Ptr<Bloomfilter> bf, int ttl);
	//PTuple(Ptr<Bloomfilter> bf, int ttl, vector<Ptr<LocalApp> >*);
	PTuple(Ptr<Bloomfilter> bf, int ttlIn,Ptr<NetDevice>  device);
	PTuple(Ptr<Bloomfilter> bf, int ttlIn,Ptr<LocalApp>  app);
	PTuple(Ptr<Bloomfilter> bf, int ttlIn, vector<Ptr<LocalApp> >* apps,Ptr<LocalApp>  app);
	PTuple(Ptr<Bloomfilter> bf, int ttlIn, vector<Ptr<LocalApp> >* apps,Ptr<NetDevice>  device);
	virtual ~PTuple();
	virtual void DoDispose(void);

	bool addLocalApp(Ptr<LocalApp> app);
	bool removeLocalApp(Ptr<LocalApp>);

	Ptr<Bloomfilter> getBF(){ return bf; }
	uint32_t getTTL(){ return ttl;}
	void setTTL(uint32_t _ttl){ ttl = _ttl;}
	vector<Ptr<LocalApp> >& getLocalApps() {return r;}//apps that asked for data
	Ptr<LocalApp> & getLocalApp() {return app;}
	Ptr<NetDevice> & getDevice() {return device;}
	void setApp(Ptr<LocalApp>);
	void setDevice(Ptr<NetDevice>);

private:
	Ptr<Bloomfilter> bf;
	uint32_t ttl;
	vector<Ptr<LocalApp> > r;

	vector<Ptr<LocalApp> >::iterator find(Ptr<LocalApp>);
	Ptr<NetDevice>  device;
	Ptr<LocalApp>  app;//app to which we forwarded
};

}
#endif /* PTUPLE_H_ */
