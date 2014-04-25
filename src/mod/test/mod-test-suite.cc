
#include "ns3/test.h"
#include "test-bloomfilter.h"
#include "test-ccn-name.h"
#include "test-ccn-packets.h"


// The TestSuite class names the TestSuite, identifies what type of TestSuite,
// and enables the TestCases to be run.  Typically, only the constructor for
// this class must be defined
//
class ModTestSuite : public TestSuite
{
public:
  ModTestSuite ();
};

ModTestSuite::ModTestSuite ()
  : TestSuite ("mod", UNIT)
{
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  //AddTestCase (new BloomfilterTestCase, TestCase::QUICK);
  //AddTestCase (new CCNNameTestCase, TestCase::QUICK);
  AddTestCase (new CCNPacketsTestCase, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static ModTestSuite modTestSuite;

