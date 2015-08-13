
#ifndef MPTCP_SCHEDULER_ROUND_ROBIN_H
#define MPTCP_SCHEDULER_ROUND_ROBIN_H


#include "ns3/mptcp-scheduler.h"
//#include "ns3/mp-tcp-socket-base.h"
//#include "ns3/mptcp-mapping.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/mptcp-scheduler-round-robin.h"
#include <vector>
#include <list>

/**
The MPTCP scheduler is in charge of selecting the
TODO: move to helpers ?
*/

namespace ns3
{

class MpTcpSocketBase;
class MpTcpSubflow;


//typedef std::vector< std::pair<uint8_t, std::pair< SequenceNumber32,uint32_t > > > MappingVector;
//
//typedef std::vector< MpTcpMapping > MappingVector;
typedef std::vector< std::pair<uint8_t, MpTcpMapping > > MappingVector;
//typedef MappingList MappingVector;


class MpTcpSchedulerRoundRobin
//: public Object
: public MpTcpScheduler
{

public:
  static TypeId
  GetTypeId (void);

  MpTcpSchedulerRoundRobin();
  virtual ~MpTcpSchedulerRoundRobin ();


  void SetMeta(Ptr<MpTcpSocketBase> metaSock);
  //Ptr<MpTcpSocketBase> metaSock
  //            std::pair<int,
            // DSNMapping
//              std::pair<SequenceNumber32,SequenceNumber32>
//            >

  /**
   * \brief This function is responsible for generating a list of packets to send
   *   and to specify on which subflow to send.
   *
   *   These *mappings* will be passed on to the meta socket that will send them without altering the
   *   mappings.
   *   It is of utmost importance to generate a perfect mapping !!! Any deviation
   *   from the foreseen mapping will trigger an error and crash the simulator
   *
   * \warn This function MUST NOT fiddle with metasockInternal
   * subflowId: pair(start,size)
   *
   * TODO should take into account backup priorities of subflows
  */
//  virtual int GenerateMappings(MappingVector& );
//  virtual int GenerateMappings(MappingVector& );
  virtual bool GenerateMapping(int& activeSubflowArrayId, SequenceNumber64& dsn, uint16_t& length) = 0;
  /**
  */
  // TODO
  // chooseSubflowForRetransmit

  /**
  Return Index of subflow to use
  */
  virtual Ptr<MpTcpSubflow> GetSubflowToUseForEmptyPacket();

protected:
  uint8_t  m_lastUsedFlowId;  //!< keep track of last used subflow
  Ptr<MpTcpSocketBase> m_metaSock;  //!<
};


} // end of 'ns3'

#endif /* MPTCP_SCHEDULER_ROUND_ROBIN_H */
