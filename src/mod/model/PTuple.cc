#include "PTuple.h"

namespace ns3 {

int PTuple::COUNT_TUPLES = 0;

/*PTuple::PTuple(Ptr<Bloomfilter> bf, int ttlIn){
	this->bf = CreateObject<Bloomfilter>(bf->getLength(), bf->getBuffer());

	ttl = ttlIn;
	r = vector<Ptr<LocalApp> >();
}

PTuple::PTuple(Ptr<Bloomfilter> bf, int ttlIn, vector<Ptr<LocalApp> >* apps) {
	this->bf = CreateObject<Bloomfilter>(bf->getLength(), bf->getBuffer());
	ttl = ttlIn;
	r = vector<Ptr<LocalApp> >();
	vector<Ptr<LocalApp> >::iterator iter;
	for(iter=apps->begin(); iter!=apps->end(); iter++){
		r.push_back(*iter);
	}
}*/

PTuple::PTuple(Ptr<Bloomfilter> bf, int ttlIn,Ptr<NetDevice>  device)
{
	COUNT_TUPLES++;
	this->bf = CreateObject<Bloomfilter>(bf->getLength(), bf->getBuffer());

	ttl = ttlIn;
	r = vector<Ptr<LocalApp> >();
	this->device=device;
	this->app=0;
}

PTuple::PTuple(Ptr<Bloomfilter> bf, int ttlIn,Ptr<LocalApp>  app)
{
	COUNT_TUPLES++;
	this->bf = CreateObject<Bloomfilter>(bf->getLength(), bf->getBuffer());

	ttl = ttlIn;
	r = vector<Ptr<LocalApp> >();
	this->device=0;
	this->app=app;
}

PTuple::PTuple(Ptr<Bloomfilter> bf, int ttlIn, vector<Ptr<LocalApp> >* apps,Ptr<LocalApp>  app)
{
	COUNT_TUPLES++;
	this->device=0;
	this->app=app;
	this->bf = CreateObject<Bloomfilter>(bf->getLength(), bf->getBuffer());
	ttl = ttlIn;
	r = vector<Ptr<LocalApp> >();
	vector<Ptr<LocalApp> >::iterator iter;
	for(iter=apps->begin(); iter!=apps->end(); iter++){
		r.push_back(*iter);
	}
}

PTuple::PTuple(Ptr<Bloomfilter> bf, int ttlIn, vector<Ptr<LocalApp> >* apps,Ptr<NetDevice>  device)
{
	COUNT_TUPLES++;
	this->device=device;
	this->app=0;
	this->bf = CreateObject<Bloomfilter>(bf->getLength(), bf->getBuffer());
	ttl = ttlIn;
	r = vector<Ptr<LocalApp> >();
	vector<Ptr<LocalApp> >::iterator iter;
	for(iter=apps->begin(); iter!=apps->end(); iter++){
		r.push_back(*iter);
	}
}

PTuple::~PTuple() {
	bf = 0;
	device = 0;
	app = 0;
}

void PTuple::DoDispose(void) {
	bf = 0;
	device = 0;
	app = 0;
	r.clear();

	COUNT_TUPLES--;

	if(COUNT_TUPLES==0)
	{
		//std::cout<<"TUPLES over."<<std::endl;
	}
}

bool PTuple::addLocalApp(Ptr<LocalApp> app){
	bool added = false;
	if (find(app) == r.end()){
		r.push_back(app);
		added = true;
	}
	return added;
}

bool PTuple::removeLocalApp(Ptr<LocalApp> app){
	bool found = false;
	vector<Ptr<LocalApp> >::iterator iter = find(app);
	if(iter != r.end()){
		r.erase(iter);
		found = true;
	}

	return found;
}

vector<Ptr<LocalApp> >::iterator PTuple::find(Ptr<LocalApp> app){
	vector<Ptr<LocalApp> >::iterator iter;
	for(iter=r.begin(); iter!=r.end(); iter++){
		if (*iter == app){
			break;
		}
	}
	return iter;
}

void PTuple::setApp(Ptr<LocalApp> app)
{
	this->app=app;
}

void PTuple::setDevice(Ptr<NetDevice> device)
{
	this->device=device;
}

}
