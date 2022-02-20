/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"



#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"




//for netanim

#include "ns3/netanim-module.h"


//for flow monitor
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

// Default Network Topology
//


using namespace ns3;
using namespace std;

const int noofFlows = 1;

vector<Ptr<PacketSink>> sink;
vector<uint64_t> lastTotalRx;                     /* The value of the last total received bytes */



std::ofstream Myfile("ownwiredtopothrough.txt");

void
CalculateThroughput (int i)
{
  
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink[i]->GetTotalRx () - lastTotalRx[i]) * (double) 8 / 1e2;     /* Convert Application RX Packets to MBits. */
  std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  if(i == 0)
      Myfile << now.GetSeconds() << "\t";
  Myfile << cur << "\t";
  if(i == noofFlows - 1)
    Myfile << "\n";
  lastTotalRx[i] = sink[i]->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput, i);
}

NS_LOG_COMPONENT_DEFINE ("NewTopoLogyScriptExample");

static bool firstCwnd = true;
static bool firstSshThr = true;
static bool firstRtt = true;

static Ptr<OutputStreamWrapper> cWndStream;
static Ptr<OutputStreamWrapper> ssThreshStream;
static Ptr<OutputStreamWrapper> rttStream;

static uint32_t cWndValue;
static uint32_t ssThreshValue;

int tracedNode = 0;


static void
CwndTracer (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd)
    {
      *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd = false;
    }
  *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << oldval << " " << newval << std::endl;
  cWndValue = newval;

  if (!firstSshThr)
    {
      *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
    }
}

static void
SsThreshTracer (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr)
    {
      *ssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr = false;
    }
  *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue = newval;

  if (!firstCwnd)
    {
      *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue << std::endl;
    }

}

static void
RttTracer (Time oldval, Time newval)
{
  if (firstRtt)
    {
      *rttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt = false;
    }
  *rttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}




static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
//   Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
  Config::ConnectWithoutContext ("/NodeList/"+std::to_string(tracedNode)+"/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
}

static void
TraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
//   Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer));
  Config::ConnectWithoutContext ("/NodeList/"+std::to_string(tracedNode)+"/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer));
}

static void
TraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  //Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer));
  Config::ConnectWithoutContext ("/NodeList/"+std::to_string(tracedNode)+"/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer));
}



int 
main (int argc, char *argv[])
{


  bool verbose = true;
  bool tracing = true;


  uint32_t payloadSize = 1024;                       /* Transport layer payload size in bytes. */
  std::string tcpVariant = "TcpNewReno";             /* TCP variant type. */

  uint32_t startTime = 0;
  uint32_t stopTime = 200; /* Simulation time in seconds. */

  std::string prefix_file_name = "LinearNet";  
  CommandLine cmd (__FILE__);
  
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  tcpVariant = std::string ("ns3::") + tcpVariant;
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));
  Config::SetDefault ("ns3::DropTailQueue<Packet>::MaxSize", QueueSizeValue (QueueSize ("27p")));
  // Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (4));
  // Config::SetDefault ("ns3::TcpSocket::InitialSlowStartThreshold", UintegerValue (20934));

  NodeContainer sender, medium, receiver;
  sender.Create(2);
  medium.Add(sender.Get(1));
  medium.Create(1);
  receiver.Add(medium.Get(1));
  receiver.Create(1);


  PointToPointHelper senderLink, mediumLink, receiverLink;

  senderLink.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  senderLink.SetChannelAttribute ("Delay", StringValue ("2ms"));

  mediumLink.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
  mediumLink.SetChannelAttribute ("Delay", StringValue ("2ms"));

  receiverLink.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  receiverLink.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer senderDevices;
  senderDevices = senderLink.Install (sender);

  NetDeviceContainer mediumDevices;
  mediumDevices = mediumLink.Install (medium);

  NetDeviceContainer receiverDevices;
  receiverDevices = receiverLink.Install (receiver);

  InternetStackHelper stack;
//   stack.Install (leftcsmaNodes);
//   stack.Install (wifiApNode);
//   stack.Install (wifiStaNodes);

  stack.Install(sender);
  stack.Install(receiver);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer senderInterfaces;
  senderInterfaces = address.Assign (senderDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer mediumInterfaces;
  mediumInterfaces = address.Assign (mediumDevices);


  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer receiverInterfaces;
  receiverInterfaces = address.Assign (receiverDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();



  // Select sender side port
  uint16_t port = 5000;



//   Here this tcp is applied

  // Install application on the receiver
  for(int i = 0; i < noofFlows; i++){

  port += i;

  //   Install application on the sender
  BulkSendHelper server ("ns3::TcpSocketFactory", InetSocketAddress (receiverInterfaces.GetAddress (1), port));
  server.SetAttribute ("MaxBytes", UintegerValue (0));
  ApplicationContainer serverApp = server.Install (sender.Get (0));
  serverApp.Start (Seconds(startTime));
    //under this commented line is not needed except schedule
//   Simulator::Schedule (Seconds (0.2), &TraceCwnd, 0, 0);
  serverApp.Stop (Seconds(stopTime));


  PacketSinkHelper mysinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = mysinkHelper.Install (receiver.Get (1));
  Ptr<PacketSink> temp = StaticCast<PacketSink> (sinkApps.Get (0));
  sink.push_back(temp);
  lastTotalRx.push_back(0);
  sinkApps.Start (Seconds(startTime));
  sinkApps.Stop (Seconds(stopTime));


  //Simulator::Schedule (Seconds (0.1), &CalculateThroughput, i);


  }
 
  




  if (tracing)
  {
      std::ofstream ascii;
      Ptr<OutputStreamWrapper> ascii_wrap;
      ascii.open ((prefix_file_name + "-ascii").c_str ());
      ascii_wrap = new OutputStreamWrapper ((prefix_file_name + "-ascii").c_str (),
                                            std::ios::out);
      stack.EnableAsciiIpv4All (ascii_wrap);

      Simulator::Schedule (Seconds (0.00001), &TraceCwnd, prefix_file_name + "-cwnd.data");
      Simulator::Schedule (Seconds (0.00001), &TraceSsThresh, prefix_file_name + "-ssth.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRtt, prefix_file_name + "-rtt.data");
  }
 
     // Flow monitor
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  

  Simulator::Stop (Seconds (stopTime));
  // Simulator::Stop (stopTime + TimeStep (1));

  //Ascii trace
//   AsciiTraceHelper ascii;
//   phyLeft.EnableAscii(ascii.CreateFileStream("phyLeftowntopo.tr"), staleftDevices.Get(1));
//   phyRight.EnableAscii(ascii.CreateFileStream("phyRightowntopo.tr"), starightDevices.Get(1));
//   routerLink.EnableAsciiAll(ascii.CreateFileStream("r2rowntopo.tr"));
//   // java -jar tracematrices.tr in tracematrices folder

//   AnimationInterface anim("owntopo_netanim.xml");
//   anim.SetConstantPosition(wifileftNodes.Get(0), 10.0, 10.0);
//   anim.SetConstantPosition(wifileftNodes.Get(1), 10.0, 30.0);
//   anim.SetConstantPosition(wifileftNodes.Get(2), 10.0, 50.0);
//   anim.SetConstantPosition(wifileftNodes.Get(3), 10.0, 70.0);
// //   anim.SetConstantPosition(wifileftNodes.Get(4), 10.0, 70.0);

//   anim.SetConstantPosition(wifirightNodes.Get(0), 40.0, 10.0);
//   anim.SetConstantPosition(wifirightNodes.Get(1), 40.0, 30.0);
//   anim.SetConstantPosition(wifirightNodes.Get(2), 40.0, 50.0);
//   anim.SetConstantPosition(wifirightNodes.Get(3), 40.0, 70.0);
// //   anim.SetConstantPosition(wifirightNodes.Get(4), 30.0, 70.0);

//   anim.SetConstantPosition(wifiApNodeleft.Get(0), 10.0, 80.0);
//   anim.SetConstantPosition(wifiApNoderight.Get(0), 40.0, 80.0);

  Simulator::Run ();
  std::vector<double> averageThroughput;
  for(int i = 0; i < noofFlows; i++)
  {
    averageThroughput.push_back((sink[i]->GetTotalRx () * 8.0)/(1e3 * stopTime));
  }
  // flowMonitor->SerializeToXmlFile("own_Linear_modifiedAlgo_topo_flow.xml", true, true);
  flowMonitor->SerializeToXmlFile("own_Linear_withoutAlgo_topo_flow.xml", true, true);
  Simulator::Destroy ();

  
  double total_throughput = 0;
  for(int i = 0; i < noofFlows; i++)
  {
    std::cout << "\nAverage throughput: (flowpair "<<i <<") "<< averageThroughput[i]<< " Kbit/s" << std::endl;
    total_throughput+=averageThroughput[i];
  }
  std::cout << "\nAverage throughput: " << (total_throughput/noofFlows) << " Kbit/s" << std::endl;
 
  return 0;
}


