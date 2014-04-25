#ifndef PIT_H_
#define PIT_H_

#include <map>
#include "CCN_Name.h"
#include "PTuple.h"
#include "local_app.h"
#include "PIT_Key.h"

using std::map;

namespace ns3 {
class PIT_Key;
class PTuple;
class Bloomfilter;

class PIT: public Object {
public:
	static int COUNT_PITS;
	PIT();
	~PIT();
	virtual void DoDispose(void);

	void update(Ptr<CCN_Name> key, Ptr<PTuple> re);

	Ptr<PTuple> check(Ptr<CCN_Name> key);
	Ptr<PTuple> checkArgs(Ptr<CCN_Name>, Ptr<LocalApp> );
	Ptr<PTuple> checkArgs(Ptr<CCN_Name>, Ptr<NetDevice> );

	void erase(Ptr<CCN_Name> key);
	void eraseArgs(Ptr<CCN_Name> key,Ptr<LocalApp>);
	void eraseArgs(Ptr<CCN_Name> key, Ptr<NetDevice>);

	uint32_t getSize();
	/*bool addRecord(Ptr<CCN_Name>, Ptr<Bloomfilter>,  uint32_t);
	bool addRecordArgs(Ptr<CCN_Name>, Ptr<NetDevice> , Ptr<Bloomfilter>,  uint32_t);
	bool addRecordArgs(Ptr<CCN_Name>, Ptr<LocalApp> , Ptr<Bloomfilter>,  uint32_t);*/
//	bool addRecordArgsMulti(Ptr<CCN_Name>, vector < Ptr<LocalApp> >, vector < Ptr<NetDevice> >, Ptr<Bloomfilter>,  uint32_t);

//private:
	map<Ptr<CCN_Name>, Ptr<PTuple> > table;
};
}

#endif
