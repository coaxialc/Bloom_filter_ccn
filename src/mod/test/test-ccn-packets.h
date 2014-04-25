/*
 * test-ccn-packets.h
 *
 *  Created on: Dec 28, 2013
 *      Author: tsilochr
 */

#ifndef TEST_CCN_PACKETS_H_
#define TEST_CCN_PACKETS_H_

#include "ns3/test.h"
#include "ns3/mod-module.h"

#include <iostream>
#include <string>
#include <vector>

using namespace ns3;
using namespace std;

class CCNPacketsTestCase: public TestCase{
public:
	CCNPacketsTestCase();
	~CCNPacketsTestCase();

private:
  virtual void DoRun (void);
  void testInterests();
  void testData();


};


CCNPacketsTestCase::CCNPacketsTestCase(): TestCase("test ccn packets"){}

CCNPacketsTestCase::~CCNPacketsTestCase(){}

void CCNPacketsTestCase::DoRun (void){
	testInterests();
	testData();
}

void CCNPacketsTestCase::testInterests(){

}

void CCNPacketsTestCase::testData(){
	Ptr<Bloomfilter> bf = CreateObject<Bloomfilter>(16);
	uint8_t ttl=2;
	
	vector<string> nameVector;
	nameVector.push_back("1");
	nameVector.push_back("2");
	nameVector.push_back("3");
	nameVector.push_back("4");
	nameVector.push_back("5");
	Ptr<CCN_Name> name = CreateObject<CCN_Name>(nameVector);
	
	const char*  msg = "hello world";
	uint8_t* buffer = (uint8_t*)msg;
	uint32_t buff_len = strlen(msg);
	CCN_Data org_data = CCN_Data(bf, ttl, name, buffer, buff_len);
	Ptr<Packet> packet = org_data.serializeToPacket();
	
	Ptr<CCN_Data> new_data = CCN_Data::deserializeFromPacket(packet);
	bool equal_names = new_data->getName()->toString().compare(name->toString()) == 0;
	NS_ASSERT_MSG(equal_names, "names not equal");

	NS_ASSERT_MSG(new_data->getHopCounter() == ttl, "ttl is " << (int)new_data->getHopCounter());
}


#endif /* TEST_CCN_PACKETS_H_ */
