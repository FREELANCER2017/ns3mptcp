/**
Should be one testsuite for TcpTxBuffer too

**/

#include "ns3/mptcp-mapping.h"
#include "ns3/log.h"
#include "ns3/test.h"


NS_LOG_COMPONENT_DEFINE ("MpTcpMappingTestSuite");

using namespace ns3;



class MpTcpMappingTestCase : public TestCase
{
public:
    MpTcpMappingTestCase (
      SequenceNumber64 headDSN, SequenceNumber32 mappedToSSN, uint16_t length,
      SequenceNumber64 headDSNbis, SequenceNumber32 mappedToSSNbis, uint16_t length2,
      bool overlap

      ) :
        TestCase("Mptcp mapping testcase"),
        //posLShd{250, 330, 512, 600, 680} // non static array initializers only available in C++11


        m_headDSN(headDSN),
        m_mappedSSN(mappedToSSN),
        m_length(length),

        m_headDSNbis(headDSNbis),
        m_mappedSSNbis(mappedToSSNbis),
        m_lengthbis(length2),

        m_shouldOverlap(overlap)
    {
        NS_LOG_FUNCTION(this);
//        m_headDSN[0] = headDSN;
//        m_headDSN[1] = headDSN2;
    }

    virtual ~MpTcpMappingTestCase ()
    {
        NS_LOG_FUNCTION(this);
    }

    virtual void DoRun(void)
    {
      MpTcpMapping map0, map1, map2;
      MpTcpMappingContainer con0,con1;

//      map0.Configure(m_headDSN, m_length);
      map0.SetHeadDSN( m_headDSN );
      map0.SetMappingSize( m_length );
      NS_TEST_EXPECT_MSG_EQ( map0.HeadDSN(), m_headDSN, "Broken setter or getter");
      NS_TEST_EXPECT_MSG_EQ( map0.GetLength(), m_length, "Broken setter");

      map0.MapToSSN( m_mappedSSN );
      map1 = map0;


      NS_TEST_EXPECT_MSG_EQ( map0.TailDSN(), m_headDSN + (uint32_t)m_length-1,"Tail DSN in included in the mapping");
      NS_TEST_EXPECT_MSG_EQ( map0.TailSSN(), m_mappedSSN + (uint32_t)m_length-1, "Tail SSN should be included in the mapping");


//      NS_TEST_ASSERT()

      NS_TEST_EXPECT_MSG_EQ(map1, map0, "Mappings have all values in common");
      map1.MapToSSN(m_headDSN - (uint32_t)m_length);
      // Not true anymore
//      NS_TEST_EXPECT_MSG_EQ(map1, map0, "Mappings should be considered equal even if they are mappped to different SSN");


      //,""
      SequenceNumber64 temp;

      NS_TEST_EXPECT_MSG_EQ(map0.TranslateSSNToDSN(map0.HeadSSN()- 4, temp), false, "ssn should not be in range");
      NS_TEST_EXPECT_MSG_EQ(map0.TranslateSSNToDSN(map0.HeadSSN(), temp), true, "ssn should be in range");
      NS_TEST_EXPECT_MSG_EQ(temp, map0.HeadDSN(), "HeadSSN must map to HeadDSN");

      NS_TEST_EXPECT_MSG_EQ(map0.TranslateSSNToDSN(map0.TailSSN() + 4, temp), false, "ssn should not be in range");
      NS_TEST_EXPECT_MSG_EQ(map0.TranslateSSNToDSN(map0.TailSSN(), temp), true, "ssn should be in range");
      NS_TEST_EXPECT_MSG_EQ(temp, map0.TailDSN(), "TailSSN must map to HeadDSN");



      /////////////////////////////////////////////////////////
      /////
      /////
      /////////////////////////////////////////////////////////

      ///// Check SSNoverlap
      ///////////////////////////////////////////
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeSSN(map0.HeadSSN()-4, 2), false,"starting at -4 with length of 2 does not overlap");
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeSSN(map0.HeadSSN(), 1), true, "HeadSSN is part of the mapping so it must overlap");
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeSSN(map0.TailSSN(), 1), true, "TailSSN is part of the mapping so it must overlap");
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeSSN(map0.TailSSN()+1, 4), false, "Data after the mapping, must not overlap");

      ///// Check DSNoverlap
      ///////////////////////////////////////////
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeDSN(map0.HeadDSN()-4, 2), false,"starting at -4 with length of 2 does not overlap");
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeDSN(map0.HeadDSN()-4, 6), true,"starting at -4 with length of 6 does not overlap");
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeDSN(map0.HeadDSN(), 1), true, "HeadDSN is part of the mapping so it must overlap");
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeDSN(map0.TailDSN(), 1), true, "TailDSN is part of the mapping so it must overlap");
      NS_TEST_EXPECT_MSG_EQ( map0.OverlapRangeDSN(map0.TailDSN()+1, 4), false, "Data after the mapping, must not overlap");


      ///// Check DSNInRange
      ///////////////////////////////////////////
      NS_TEST_EXPECT_MSG_EQ( map0.IsDSNInRange(map0.HeadDSN()), true, "test ");
      NS_TEST_EXPECT_MSG_EQ( map0.IsDSNInRange(map0.TailDSN()), true, "");
      NS_TEST_EXPECT_MSG_EQ( map0.IsDSNInRange(map0.HeadDSN()-1 ), false, "");
      NS_TEST_EXPECT_MSG_EQ( map0.IsDSNInRange(map0.TailDSN()+1 ), false,"");

      ///// Check SSNInRange
      ///////////////////////////////////////////
      // TODO maybe do some of the tests only
      NS_TEST_EXPECT_MSG_EQ( map0.IsSSNInRange(map0.HeadSSN()), true, "First byte is part of the mapping");
      NS_TEST_EXPECT_MSG_EQ( map0.IsSSNInRange(map0.TailSSN()), true, "Last byte is part of the mapping");
      NS_TEST_EXPECT_MSG_EQ( map0.IsSSNInRange(map0.HeadSSN()-1), false, "Byte just before first byte is not part of the mapping");
      NS_TEST_EXPECT_MSG_EQ( map0.IsSSNInRange(map0.TailSSN()+1), false, "Byte just after last byte is not part of the mapping");







//      con0.AddMapping(map0);
      MpTcpMapping ret, overlap;
//      overlap.SetHeadDSN(map0.HeadDSN());
//      overlap.SetMappingSize(map0.GetLength() + 4);
      overlap.SetHeadDSN(m_headDSNbis);
      overlap.SetMappingSize(m_lengthbis);
      overlap.MapToSSN(m_mappedSSNbis);

      NS_TEST_EXPECT_MSG_EQ(con0.AddMapping(map0), true, "This is the first mapping registered");
      NS_TEST_EXPECT_MSG_EQ(con1.AddMapping(overlap), true, "This is the first mapping registered");

//      NS_TEST_EXPECT_MSG_EQ(con0.FindOverlappingMapping(map0, false, ret), true, "Don't ignore identical mappings");
//      NS_TEST_EXPECT_MSG_EQ(ret, map0, "Previous test should have filled ret with mapping equal to map0");

//      NS_TEST_EXPECT_MSG_EQ(con0.FindOverlappingMapping(overlap, false, ret), m_shouldOverlap, "Ignore identical mappings so it should find none");
//      NS_TEST_EXPECT_MSG_EQ(con1.FindOverlappingMapping(map0, false, ret), m_shouldOverlap, "Ignore identical mappings so it should find none");




      //! Now we add an overlapping mapping

//      NS_TEST_EXPECT_MSG_EQ(con0.AddMapping(map0), false, "They should intersect");

//        TestSerialize();
//        TestDeserialize();
    }
protected:
  const SequenceNumber64 m_headDSN;
  const SequenceNumber32 m_mappedSSN;
  const uint16_t m_length;

  const SequenceNumber64 m_headDSNbis;
  const SequenceNumber32 m_mappedSSNbis;
  const uint16_t m_lengthbis;


  const bool m_shouldOverlap;
};

/**

*/
class MpTcpMappingContainerTestCase : public TestCase
{
public:
    MpTcpMappingContainerTestCase () : TestCase("Mptcp mapping testcase")
    {
        NS_LOG_FUNCTION(this);
    }

    virtual ~MpTcpMappingContainerTestCase ()
    {
        NS_LOG_FUNCTION(this);
    }

    virtual void DoRun(void);
};

void
MpTcpMappingContainerTestCase::DoRun()
{
  MpTcpMapping map0, map1, map2;
  MpTcpMappingContainer con0,con1;

  map0.SetHeadDSN( SequenceNumber64(40));
  map0.SetMappingSize(10);
  map0.MapToSSN( SequenceNumber32(32));

  map1.SetHeadDSN( SequenceNumber64(50) );
  map1.SetMappingSize(10);
  map1.MapToSSN( map0.TailSSN()+1);


  map2.SetHeadDSN( SequenceNumber64(60) );
  map2.SetMappingSize(10);
  map2.MapToSSN( map0.TailSSN()+ map1.GetLength());

//      con0.AddMapping(map0);
  MpTcpMapping ret, overlap;
//      overlap.SetHeadDSN(map0.HeadDSN());
//      overlap.SetMappingSize(map0.GetLength() + 4);
//  overlap.SetHeadDSN(m_headDSNbis);
//  overlap.SetMappingSize(m_lengthbis);
//  overlap.MapToSSN(m_mappedSSNbis);

  SequenceNumber32 ssn;

  // Test on empty container
  NS_TEST_EXPECT_MSG_EQ(con0.FirstUnmappedSSN(ssn), false, "An empty container can't return the unmapped ssn");
  NS_TEST_EXPECT_MSG_EQ(con0.GetMappingForSSN(map0.HeadSSN(), ret), false, "An empty container can't find a mapping.");

  // Tests with one mapping
  NS_TEST_EXPECT_MSG_EQ(con0.AddMapping(map0), true, "This is the first mapping registered");
//  NS_TEST_EXPECT_MSG_EQ(con0.AddMapping(map0), , "This is the first mapping registered");

  NS_TEST_EXPECT_MSG_EQ(con0.FirstUnmappedSSN(ssn), true, "An empty container can't return the unmapped ssn");
  NS_TEST_EXPECT_MSG_EQ(ssn, map0.TailSSN()+1, "Now that there is a mapping, it should be able to return a result");

//  for(SequenceNumber32 i(0); i < map0.TailSSN(); ++i)
//  {
    NS_TEST_EXPECT_MSG_EQ(con0.GetMappingForSSN(map0.TailSSN(), ret), true, "Now that there is a mapping, it should be able to return a result");
    NS_TEST_EXPECT_MSG_EQ(ret, map0, "The returnd mapping should be equal to the one inserted");
//  }

  NS_TEST_EXPECT_MSG_EQ(con0.DiscardMapping(map1), false, "This mapping was not registered");
  NS_TEST_EXPECT_MSG_EQ(con0.DiscardMapping(map0), true, "This mapping was registered");

  NS_TEST_EXPECT_MSG_EQ(con0.GetMappingForSSN(map0.HeadSSN(), ret), false, "An empty container can't find a mapping.");

//  NS_TEST_EXPECT_MSG_EQ(con0.GetMappingForSSN(map0.HeadSSN(), ret), true, "Now that there is a mapping, it should be able to return a result");
  // TODO do a test to check that we get all

  // tests with 2 non contiguous mappings
  NS_TEST_EXPECT_MSG_EQ(con1.AddMapping(map0), true, "");
  NS_TEST_EXPECT_MSG_EQ(con1.AddMapping(map2), true, "");

  NS_TEST_EXPECT_MSG_EQ(con1.GetMappingForSSN(map0.TailSSN() , ret), true, "This ssn should exist in the container");
  NS_TEST_EXPECT_MSG_EQ(con1.GetMappingForSSN(map1.HeadSSN(), ret), false, "This ssn should not exist in the container");
//  NS_TEST_EXPECT_MSG_EQ(con1.AddMapping(map1), true, "");
//  con1.AddMapping(map0);



  //!

//  NS_TEST_EXPECT_MSG_EQ(con0.AddMapping(map0), true, "This is the first mapping registered");
//  NS_TEST_EXPECT_MSG_EQ(con1.AddMapping(overlap), true, "This is the first mapping registered");
//
//  NS_TEST_EXPECT_MSG_EQ(con0.FindOverlappingMapping(map0, false, ret), true, "Don't ignore identical mappings");
//  NS_TEST_EXPECT_MSG_EQ(ret, map0, "Previous test should have filled ret with mapping equal to map0");
//
////      NS_TEST_EXPECT_MSG_EQ(con0.FindOverlappingMapping(overlap, false, ret), m_shouldOverlap, "Ignore identical mappings so it should find none");
//  NS_TEST_EXPECT_MSG_EQ(con1.FindOverlappingMapping(map0, false, ret), m_shouldOverlap, "Ignore identical mappings so it should find none");

  // GetMappingFor SSN


  //! Now we add an overlapping mapping

//      NS_TEST_EXPECT_MSG_EQ(con0.AddMapping(map0), false, "They should intersect");

//        TestSerialize();
//        TestDeserialize();
}



static class MpTcpMappingTestSuite : public TestSuite
{
public:
 MpTcpMappingTestSuite ()
 : TestSuite ("mptcp-mapping", UNIT)
 {
//    for (uint8_t i=0; i< 40; i += 10)
//    {

        ////////////////////////////////////////////////
        //// HeadDSN/headSSN/Length x 2  / shouldoverlap

        AddTestCase(new MpTcpMappingTestCase(
                SequenceNumber64(10), SequenceNumber32(60),40,
                SequenceNumber64(10), SequenceNumber32(60),40,
                true), QUICK);

        AddTestCase(new MpTcpMappingTestCase(
            SequenceNumber64(50), SequenceNumber32(1), 80,
            SequenceNumber64(100), SequenceNumber32(10),800,
            true), QUICK);

        AddTestCase(new MpTcpMappingTestCase(
            SequenceNumber64(50), SequenceNumber32(1), 40,
            SequenceNumber64(90), SequenceNumber32(80), 1000,
            false), QUICK);

//  Mapping DSN [6560-6659] mapped to SSN [6561-6660]
//     with DSN [6660-6759] mapped to SSN [6661-6760]
        AddTestCase(new MpTcpMappingTestCase(
          SequenceNumber64(6560), SequenceNumber32(6561), 100,
          SequenceNumber64(6660), SequenceNumber32(6661), 100,
          false), QUICK);

          // Mapping container tests
        AddTestCase(new MpTcpMappingContainerTestCase(

          ), QUICK);
 }

//
//
//
} g_MpTcpMappingTestSuite;
