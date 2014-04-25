/*
 * test-ccn-name.h
 *
 *  Created on: Dec 28, 2013
 *      Author: tsilochr
 */

#ifndef TEST_CCN_NAME_H_
#define TEST_CCN_NAME_H_


#include "ns3/test.h"
#include "ns3/mod-module.h"

#include <iostream>
#include <string>

using namespace ns3;
using namespace std;

class CCNNameTestCase : public TestCase
{
public:
	CCNNameTestCase ();
	virtual ~CCNNameTestCase ();

private:
	void testToString();
	void testSerializers();

private:
  virtual void DoRun (void);
};

CCNNameTestCase::CCNNameTestCase (): TestCase("CCNName test case"){}

CCNNameTestCase::~CCNNameTestCase (){}

void
CCNNameTestCase::DoRun (void){
	testToString();
	testSerializers();
}

void
CCNNameTestCase::testToString(){
	string nameStr = "/1/2/3/4/5";
	vector<string> nameVector;
	nameVector.push_back("1");
	nameVector.push_back("2");
	nameVector.push_back("3");
	nameVector.push_back("4");
	nameVector.push_back("5");

	Ptr<CCN_Name> name = CreateObject<CCN_Name>(nameVector);
	string str = name->toString();
	cout<< "ccn name to string " << str << endl;
	NS_TEST_ASSERT_MSG_EQ(str, nameStr, "check toString()");
}

void
CCNNameTestCase::testSerializers(){
	vector<string> nameVector;
	nameVector.push_back("foo");
	nameVector.push_back("moo");
	nameVector.push_back("voo");

	Ptr<CCN_Name> name = CreateObject<CCN_Name>(nameVector);
	uint32_t buffsize = name->serializedSize();
	uint8_t *buff = (uint8_t*)malloc(buffsize*sizeof(uint8_t));

	uint32_t wrote = name->serializeToBuffer(buff);
	NS_TEST_ASSERT_MSG_EQ(buffsize, wrote, "check serialized");

	pair<Ptr<CCN_Name>, uint32_t> p = CCN_Name::deserializeFromBuffer(buff);
	cout << "deserialized " << p.first->toString() << endl;
	NS_TEST_ASSERT_MSG_EQ(buffsize, p.second, "check deserialized read bytes");
	NS_TEST_ASSERT_MSG_EQ(name, p.first, "check deserialized read name");
	free(buff);


}


#endif /* TEST_CCN_NAME_H_ */
