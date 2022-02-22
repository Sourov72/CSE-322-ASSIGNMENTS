/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Natale Patriciello <natale.patriciello@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "tcp-congestion-ops.h"
#include "ns3/log.h"
#include <stdint.h>
namespace ns3
{

  NS_LOG_COMPONENT_DEFINE("TcpCongestionOps");

  NS_OBJECT_ENSURE_REGISTERED(TcpCongestionOps);

  TypeId
  TcpCongestionOps::GetTypeId(void)
  {
    static TypeId tid = TypeId("ns3::TcpCongestionOps")
                            .SetParent<Object>()
                            .SetGroupName("Internet");
    return tid;
  }

  TcpCongestionOps::TcpCongestionOps() : Object()
  {
  }

  TcpCongestionOps::TcpCongestionOps(const TcpCongestionOps &other) : Object(other)
  {
  }

  TcpCongestionOps::~TcpCongestionOps()
  {
  }

  void
  TcpCongestionOps::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
  {
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);
  }

  void
  TcpCongestionOps::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                              const Time &rtt)
  {
    NS_LOG_FUNCTION(this << tcb << segmentsAcked << rtt);
  }

  void
  TcpCongestionOps::CongestionStateSet(Ptr<TcpSocketState> tcb,
                                       const TcpSocketState::TcpCongState_t newState)
  {
    NS_LOG_FUNCTION(this << tcb << newState);
  }

  void
  TcpCongestionOps::CwndEvent(Ptr<TcpSocketState> tcb,
                              const TcpSocketState::TcpCAEvent_t event)
  {
    NS_LOG_FUNCTION(this << tcb << event);
  }

  bool
  TcpCongestionOps::HasCongControl() const
  {
    return false;
  }

  void
  TcpCongestionOps::CongControl(Ptr<TcpSocketState> tcb,
                                const TcpRateOps::TcpRateConnection &rc,
                                const TcpRateOps::TcpRateSample &rs)
  {
    NS_LOG_FUNCTION(this << tcb);
    NS_UNUSED(rc);
    NS_UNUSED(rs);
  }

  // RENO

  NS_OBJECT_ENSURE_REGISTERED(TcpNewReno);

  TypeId
  TcpNewReno::GetTypeId(void)
  {
    static TypeId tid = TypeId("ns3::TcpNewReno")
                            .SetParent<TcpCongestionOps>()
                            .SetGroupName("Internet")
                            .AddConstructor<TcpNewReno>();
    return tid;
  }

  TcpNewReno::TcpNewReno(void) : TcpCongestionOps()
  {
    NS_LOG_FUNCTION(this);
  }

  TcpNewReno::TcpNewReno(const TcpNewReno &sock)
      : TcpCongestionOps(sock)
  {
    NS_LOG_FUNCTION(this);
  }

  TcpNewReno::~TcpNewReno(void)
  {
  }

  /**
   * \brief Tcp NewReno slow start algorithm
   *
   * Defined in RFC 5681 as
   *
   * > During slow start, a TCP increments cwnd by at most SMSS bytes for
   * > each ACK received that cumulatively acknowledges new data.  Slow
   * > start ends when cwnd exceeds ssthresh (or, optionally, when it
   * > reaches it, as noted above) or when congestion is observed.  While
   * > traditionally TCP implementations have increased cwnd by precisely
   * > SMSS bytes upon receipt of an ACK covering new data, we RECOMMEND
   * > that TCP implementations increase cwnd, per:
   * >
   * >    cwnd += min (N, SMSS)                      (2)
   * >
   * > where N is the number of previously unacknowledged bytes acknowledged
   * > in the incoming ACK.
   *
   * The ns-3 implementation respect the RFC definition. Linux does something
   * different:
   * \verbatim
  u32 tcp_slow_start(struct tcp_sock *tp, u32 acked)
    {
      u32 cwnd = tp->snd_cwnd + acked;

      if (cwnd > tp->snd_ssthresh)
        cwnd = tp->snd_ssthresh + 1;
      acked -= cwnd - tp->snd_cwnd;
      tp->snd_cwnd = min(cwnd, tp->snd_cwnd_clamp);

      return acked;
    }
    \endverbatim
   *
   * As stated, we want to avoid the case when a cumulative ACK increases cWnd more
   * than a segment size, but we keep count of how many segments we have ignored,
   * and return them.
   *
   * \param tcb internal congestion state
   * \param segmentsAcked count of segments acked
   * \return the number of segments not considered for increasing the cWnd
   */
  uint32_t
  TcpNewReno::SlowStart(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
  {
  //   NS_LOG_FUNCTION(this << tcb << segmentsAcked);
  //  // NS_LOG_UNCOND("last rtt -> " << tcb->m_lastRtt / 1000000.0);
  //   tcb->m_totalrtt += (tcb->m_lastRtt / 1000000.0);
    tcb->count += 1;
  //   //NS_LOG_UNCOND("total rtt " << tcb->m_totalrtt);
  //   //NS_LOG_UNCOND("min rtt " << tcb->m_minRtt / 1000000);
    NS_LOG_UNCOND("congestion window " << tcb->m_cWnd);
  //   NS_LOG_UNCOND("initial threshold value --> " << tcb->m_initialSsThresh);
    NS_LOG_UNCOND("threshold value --> " << tcb->m_ssThresh);
    
    int algo = 1;

    if (algo == 1)
    {
      if (segmentsAcked >= 1)
      {
      
        
        uint64_t  val = (((uint64_t)tcb->m_ssThresh + 1) / 2);
        NS_LOG_UNCOND("val : " << val);
        NS_LOG_UNCOND("logvalue : " << log2(val));
        NS_LOG_UNCOND("count " << tcb->count);
        if (tcb->count <= log2(val))
        {
         
          NS_LOG_UNCOND("come in exponential increase part");
          tcb->m_cWnd = pow(2, tcb->count);
          NS_LOG_UNCOND("here the count is " << tcb->count);
          NS_LOG_UNCOND("here the congestion window is " << tcb->m_cWnd);
        }
        else
        {
          NS_LOG_UNCOND("come in slow exponential increase part");
          uint64_t tempthresh = (uint64_t )tcb->m_ssThresh + 1;
          NS_LOG_UNCOND("temp thresh " << tempthresh);
          uint64_t val = (tempthresh * (tempthresh / 4));
          NS_LOG_UNCOND("val ex " << val);
          // val = val / 4;
          int power = tcb->count / 32;
          int rem = tcb->count - (power * 32);

          NS_LOG_UNCOND("pow " << power);
          NS_LOG_UNCOND("rem " << rem);

          while(power){
            uint64_t power2 = pow(2, 32);
            val = val / power2;
            power--;
          }
          
          val = val / (pow(2, rem));

          NS_LOG_UNCOND("value of slow ex: " <<  val);
          tcb->m_cWnd = ((uint64_t)tcb->m_ssThresh + 1 - val);
        }

       NS_LOG_UNCOND("congestion window 2 " << tcb->m_cWnd << "\n\n\n");
        return segmentsAcked - 1;
      }
      else
        return 0;
      
    }

    else
    {

      if (segmentsAcked >= 1)
      {
        tcb->m_cWnd += tcb->m_segmentSize;
        // tcb->m_cWnd *= 2;
        NS_LOG_INFO("In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh " << tcb->m_ssThresh);
        return segmentsAcked - 1;
      }
    }
    return 0;
  }

  /**
   * \brief NewReno congestion avoidance
   *
   * During congestion avoidance, cwnd is incremented by roughly 1 full-sized
   * segment per round-trip time (RTT).
   *
   * \param tcb internal congestion state
   * \param segmentsAcked count of segments acked
   */
  void
  TcpNewReno::CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
  {
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    if (segmentsAcked > 0)
    {
      double adder = static_cast<double>(tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get();
      adder = std::max(1.0, adder);
      tcb->m_cWnd += static_cast<uint32_t>(adder);
      NS_LOG_INFO("In CongAvoid, updated to cwnd " << tcb->m_cWnd << " ssthresh " << tcb->m_ssThresh);
    }
  }

  /**
   * \brief Try to increase the cWnd following the NewReno specification
   *
   * \see SlowStart
   * \see CongestionAvoidance
   *
   * \param tcb internal congestion state
   * \param segmentsAcked count of segments acked
   */
  void
  TcpNewReno::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
  {

    // NS_LOG_UNCOND ("hello it's in new reno increase window here");
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);
    // NS_LOG_UNCOND("the congestion window size in function calling " << tcb->m_cWnd);
    // NS_LOG_UNCOND("the threshold value in function calling " << tcb->m_ssThresh);
    if (tcb->m_cWnd < tcb->m_ssThresh)
    {
     NS_LOG_UNCOND("in slow start mode");

      segmentsAcked = SlowStart(tcb, segmentsAcked);
    }

    if (tcb->m_cWnd >= tcb->m_ssThresh)
    {
      // NS_LOG_UNCOND("in congestion avoidance mode");
      CongestionAvoidance(tcb, segmentsAcked);
    }

    /* At this point, we could have segmentsAcked != 0. This because RFC says
     * that in slow start, we should increase cWnd by min (N, SMSS); if in
     * slow start we receive a cumulative ACK, it counts only for 1 SMSS of
     * increase, wasting the others.
     *
     * // Incorrect assert, I am sorry
     * NS_ASSERT (segmentsAcked == 0);
     */
  }

  std::string
  TcpNewReno::GetName() const
  {
    return "TcpNewReno";
  }

  uint32_t
  TcpNewReno::GetSsThresh(Ptr<const TcpSocketState> state,
                          uint32_t bytesInFlight)
  {
    NS_LOG_FUNCTION(this << state << bytesInFlight);

    return std::max(2 * state->m_segmentSize, bytesInFlight / 2);
  }

  Ptr<TcpCongestionOps>
  TcpNewReno::Fork()
  {
    return CopyObject<TcpNewReno>(this);
  }

} // namespace ns3
