/*
 * test-bloomfilter.h
 *
 *  Created on: Dec 28, 2013
 *      Author: tsilochr
 */

#ifndef TEST_BLOOMFILTER_H_
#define TEST_BLOOMFILTER_H_

#include "ns3/test.h"
#include "ns3/mod-module.h"

#include <iostream>
#include <string>


// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;
using namespace std;

// This is an example TestCase.
class BloomfilterTestCase : public TestCase
{
public:
	BloomfilterTestCase ();
  virtual ~BloomfilterTestCase ();

private:
  virtual void DoRun (void);
  void testToString();
  void testAND_OR();
  void test_operators();
  void test_serializers();
};

// Add some help text to this case to describe what it is intended to test
BloomfilterTestCase::BloomfilterTestCase ()
  : TestCase ("BloomfilterTestCase test case")
{
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
BloomfilterTestCase::~BloomfilterTestCase ()
{
}

//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
BloomfilterTestCase::DoRun (void)
{
  testToString();
  testAND_OR();
  test_operators();
  test_serializers();
}

void
BloomfilterTestCase::testToString(){
	Ptr<Bloomfilter> bf = CreateObject<Bloomfilter>(2);
	string str = bf->toString();
	string correct = "0000000000000000";
	NS_TEST_ASSERT_MSG_EQ (str, correct, "incorrect toString");

	bf->setBit(15);
	str = bf->toString();
	cout << str << endl;

	correct[15] = '1';
	cout << correct << endl;
	NS_TEST_ASSERT_MSG_EQ (str, correct, "incorrect toString");

	bf->setBit(0);
	correct[0] = '1';
	str = bf->toString();
	NS_TEST_ASSERT_MSG_EQ (str, correct, "incorrect toString");
}

void
BloomfilterTestCase::testAND_OR(){
	Ptr<Bloomfilter> bf1 = CreateObject<Bloomfilter>(2);
	Ptr<Bloomfilter> bf2 = CreateObject<Bloomfilter>(2);

	bf2->setBit(1);
	bf2->setBit(7);
	bf2->setBit(8);

	bf1->OR(bf2);
	bool contains = bf1->contains(bf2);
	NS_TEST_ASSERT_MSG_EQ (contains, true, "incorrect OR/contains");

	Ptr<Bloomfilter> bf3 = CreateObject<Bloomfilter>(2);
	bf3->setBit(14);
	contains = bf1->contains(bf3);
	NS_TEST_ASSERT_MSG_EQ (contains, false, "incorrect OR/contains");
}

void
BloomfilterTestCase::test_operators(){
	Ptr<Bloomfilter> bf1 = CreateObject<Bloomfilter>(2);
	Ptr<Bloomfilter> bf2 = CreateObject<Bloomfilter>(2);
	bf2->setBit(7);

	bool bf1smaller = bf1 < bf2;
	NS_TEST_ASSERT_MSG_EQ (bf1smaller, true, "incorrect operator<");

	bool equals = bf1 == bf2;
	NS_TEST_ASSERT_MSG_EQ (equals, false, "incorrect operator==");

	bf1->setBit(7);
	equals = bf1 == bf2;
	NS_TEST_ASSERT_MSG_EQ (equals, true, "incorrect operator==");
}

void
BloomfilterTestCase::test_serializers(){
	Bloomfilters::BF_LENGTH = 2;

	Ptr<Bloomfilter> bf1 = CreateObject<Bloomfilter>(2);
	bf1->setBit(0);
	bf1->setBit(2);
	bf1->setBit(7);

	uint32_t size = bf1->serializedSize();
	uint8_t *buff = (uint8_t*)malloc(size*sizeof(uint8_t));
	uint32_t wrote = bf1->serializeToBuffer(buff);
	NS_TEST_ASSERT_MSG_EQ (size, wrote, "check serialized");

	pair<Ptr<Bloomfilter>, uint32_t> bf2 = Bloomfilter::deserializeFromBuffer(buff);
	NS_TEST_ASSERT_MSG_EQ (wrote, bf2.second, "check serialized read bytes");
	NS_TEST_ASSERT_MSG_EQ (bf1, bf2.first, "check deserialized bf");

	free(buff);

}

#endif /* TEST_BLOOMFILTER_H_ */
