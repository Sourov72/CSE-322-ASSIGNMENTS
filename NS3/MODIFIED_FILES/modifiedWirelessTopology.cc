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


uint32_t noofFlows = 15;
vector<Ptr<PacketSink>> sink;
vector<uint64_t> lastTotalRx;                     /* The value of the last total received bytes */

std::ofstream Myfile("modifiedWirelessTopology.txt");

void
CalculateThroughput (uint32_t i)
{
  
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink[i]->GetTotalRx () - lastTotalRx[i]) * (double) 8 / 1e2;     /* Convert Application RX Packets to MBits. */
  std::cout << now.GetSeconds () << "s: \t" << cur << " Kbit/s" << std::endl;
  if(i == 0)
      Myfile << now.GetSeconds() << "\t";
  Myfile << cur << "\t";
  if(i == noofFlows - 1)
    Myfile << "\n";
  lastTotalRx[i] = sink[i]->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput, i);
}

NS_LOG_COMPONENT_DEFINE ("NewTopoLogyScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  bool tracing = true;
  uint32_t nWifileft = 14;
  uint32_t nWifiright = 14;

//   uint32_t packets_per_sec = 100;
 



  uint32_t payloadSize = 1024;                       /* Transport layer payload size in bytes. */
  std::string dataRate = "512Kbps";                  /* Application layer datarate. */
  std::string tcpVariant = "TcpNewReno";             /* TCP variant type. */
  std::string phyRate = "HtMcs7";                    /* Physical layer bitrate. */
  string queueDisc ="FifoQueueDisc";                       
  uint32_t delAckCount = 2;
  uint32_t noofpacketspersec = 500;
  Time startTime = Seconds(0);
  Time stopTime = Seconds(20); /* Simulation time in seconds. */
  

  CommandLine cmd (__FILE__);
  
  cmd.AddValue ("nWifileft", "Number of wifiLeft STA devices left", nWifileft);
  cmd.AddValue ("nWifiright", "Number of wifiLeft STA devices right", nWifiright);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("noofFlows", "no of flows in network", noofFlows);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.AddValue ("noofpacketspersec", "no of packets per second", noofpacketspersec);

  cmd.Parse (argc,argv);

  tcpVariant = std::string ("ns3::") + tcpVariant;
  queueDisc = string("ns3::") + queueDisc;

  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (4194304));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (6291456));
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (2));
  Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (delAckCount));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));
  Config::SetDefault ("ns3::DropTailQueue<Packet>::MaxSize", QueueSizeValue (QueueSize ("50p")));
  Config::SetDefault (queueDisc + "::MaxSize", QueueSizeValue (QueueSize ("100p")));


  // cout << nWifileft << endl;
  // cout << nWifiright << endl;
  // cout << noofFlows << endl;
  cout << noofpacketspersec << endl;
  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
//   if (nWifileft > 18)
//     {
//       std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
//       return 1;
//     }

//   if (verbose)
//     {
//       LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
//       LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
//     }

  NodeContainer routers;
  NodeContainer wifileftNodes, wifirightNodes;
  routers.Create (2);
  wifileftNodes.Create(nWifileft);
  wifirightNodes.Create(nWifiright);

  PointToPointHelper routerLink;
  routerLink.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  routerLink.SetChannelAttribute ("Delay", StringValue ("20ms"));
  

  NetDeviceContainer r2rDevices;
  r2rDevices = routerLink.Install (routers);

  NodeContainer wifiApNodeleft = routers.Get (0);
  NodeContainer wifiApNoderight = routers.Get (1);

    //left wifi node connection
  WifiHelper wifiLeft;

  YansWifiChannelHelper channelLeft;
  channelLeft.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channelLeft.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (5 * nWifileft));
  //YansWifiChannelHelper channelLeft = YansWifiChannelHelper::Default ();


  YansWifiPhyHelper phyLeft;

  phyLeft.SetChannel (channelLeft.Create ());
//   phyLeft.SetErrorRateModel ("ns3::YansErrorRateModel");
//   wifiLeft.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
//                                       "DataMode", StringValue (phyRate),
//                                       "ControlMode", StringValue ("HtMcs0"));


  
 // wifiLeft.SetStandard (WIFI_STANDARD_80211n_5GHZ);
  wifiLeft.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper macleft;
  Ssid ssidLeft = Ssid ("ns-3-ssidLeft");
  macleft.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssidLeft),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staleftDevices;
  staleftDevices = wifiLeft.Install (phyLeft, macleft, wifileftNodes);

  macleft.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssidLeft));

  NetDeviceContainer apleftDevices;
  apleftDevices = wifiLeft.Install (phyLeft, macleft, wifiApNodeleft);


  MobilityHelper mobilityleft;

  mobilityleft.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (10.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobilityleft.SetMobilityModel ("ns3::ConstantPositionMobilityModel");                               
  mobilityleft.Install(wifiApNodeleft);

//   mobilityleft.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
//                              "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

  Ptr<ListPositionAllocator> subnetAllocl = CreateObject<ListPositionAllocator> ();
  for(uint32_t k = 0; k < wifileftNodes.GetN(); ++k){

    subnetAllocl -> Add(Vector(0.0, 5 + (3*k), 0.0));

  }
  mobilityleft.PushReferenceMobilityModel(wifiApNodeleft.Get(0));
  mobilityleft.SetPositionAllocator(subnetAllocl);
  mobilityleft.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityleft.Install (wifileftNodes);




    // right wifi node connection

  WifiHelper wifiRight;
  //wifiRight.SetStandard (WIFI_STANDARD_80211n_5GHZ);
  wifiRight.SetRemoteStationManager("ns3::AarfWifiManager");



  YansWifiChannelHelper channelRight;

  channelRight.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channelRight.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (5 * nWifiright));
    //YansWifiChannelHelper channelRight = YansWifiChannelHelper::Default();


  YansWifiPhyHelper phyRight;
  phyRight.SetChannel (channelRight.Create ());
//   phyRight.SetErrorRateModel ("ns3::YansErrorRateModel");
//   wifiRight.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
//                                       "DataMode", StringValue (phyRate),
//                                       "ControlMode", StringValue ("HtMcs0"));

  // wifiRight.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  //                                     "DataMode", StringValue ("OfdmRate6Mbps");
 
  // wifiRight.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper macRight;
  Ssid ssidRight = Ssid ("ns-3-ssidRight");
  macRight.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssidRight),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer starightDevices;
  starightDevices = wifiRight.Install (phyRight, macRight, wifirightNodes);

  macRight.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssidRight));

  NetDeviceContainer aprightDevices;
  aprightDevices = wifiRight.Install (phyRight, macRight, wifiApNoderight);




  MobilityHelper mobilityright;

  mobilityright.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (10.0),
                                 "MinY", DoubleValue (10.0),
                                 "DeltaX", DoubleValue (20.0),
                                 "DeltaY", DoubleValue (20.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobilityright.SetMobilityModel ("ns3::ConstantPositionMobilityModel");                               
  mobilityright.Install(wifiApNoderight);

//   mobilityleft.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
//                              "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

  Ptr<ListPositionAllocator> subnetAllocr = CreateObject<ListPositionAllocator> ();
  for(uint32_t k = 0; k < wifirightNodes.GetN(); ++k){

    subnetAllocr -> Add(Vector(1.0, 5 + (3*k), 0.0));

  }
  mobilityright.PushReferenceMobilityModel(wifiApNoderight.Get(0));
  mobilityright.SetPositionAllocator(subnetAllocr);
  mobilityright.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityright.Install (wifirightNodes);



 //stack work

  InternetStackHelper stack;
//   stack.Install (csmaNodes);
//   stack.Install (wifiApNode);
//   stack.Install (wifiStaNodes);

    stack.Install(wifiApNodeleft);
    stack.Install(wifiApNoderight);
    stack.Install(wifileftNodes);
    stack.Install(wifirightNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer r2rInterfaces;
  r2rInterfaces = address.Assign (r2rDevices);

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer leftInterfaces;
  leftInterfaces = address.Assign (staleftDevices);
  Ipv4InterfaceContainer leftap;
  leftap = address.Assign (apleftDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer rightInterfaces;
  rightInterfaces = address.Assign (starightDevices);
  Ipv4InterfaceContainer rightap;
  rightap = address.Assign (aprightDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();



  // Select sender side port
  uint16_t port = 5000;




////// Here this tcp is applied

  // Install application on the receiver
  for(uint32_t i = 0; i < noofFlows / 2; i++){

  srand(time(0));
  int sender = (rand() % nWifileft);
  int receiver = (rand() % nWifiright);
  port += i;

  //   Install application on the sender
//   BulkSendHelper server ("ns3::TcpSocketFactory", InetSocketAddress (rightInterfaces.GetAddress (receiver), port));
//   server.SetAttribute ("MaxBytes", UintegerValue (0));
//   ApplicationContainer serverApp = server.Install (wifileftNodes.Get (sender));
//   serverApp.Start (startTime);
//     //under this commented line is not needed except schedule
// //   Simulator::Schedule (Seconds (0.2), &TraceCwnd, 0, 0);
//   serverApp.Stop (stopTime);
  
  OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (rightInterfaces.GetAddress (receiver), port)));
  server.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  server.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  server.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  // server.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
  server.SetConstantRate (DataRate (noofpacketspersec*payloadSize*8));
  
  ApplicationContainer serverApp = server.Install (wifileftNodes.Get (sender));
  serverApp.Start(startTime);
  serverApp.Stop(stopTime);



  PacketSinkHelper mysinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = mysinkHelper.Install (wifirightNodes.Get (receiver));
  Ptr<PacketSink> temp = StaticCast<PacketSink> (sinkApps.Get (0));
  sink.push_back(temp);
  lastTotalRx.push_back(0);
  sinkApps.Start (startTime);
  sinkApps.Stop (stopTime);

  Simulator::Schedule (Seconds (0.1), &CalculateThroughput, i);


  }
 
//   port = 5000;

for(uint32_t i = noofFlows /2; i < noofFlows; i++){

  port += i;
  srand(time(0));
  int sender = (rand() % nWifileft);
  int receiver = (rand() % nWifiright);
  //   Install application on the sender
  // BulkSendHelper server ("ns3::TcpSocketFactory", InetSocketAddress (leftInterfaces.GetAddress (receiver), port));
  // server.SetAttribute ("MaxBytes", UintegerValue (0));
  // ApplicationContainer serverApp = server.Install (wifirightNodes.Get (sender));
  // //under this commented line is not needed except schedule
  // serverApp.Start (startTime);
  // //Simulator::Schedule (Seconds (0.2), &TraceCwnd, 0, 0);
  // serverApp.Stop (stopTime);  


  OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (leftInterfaces.GetAddress (receiver), port)));
  server.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  server.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  server.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  // server.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
  server.SetConstantRate (DataRate (noofpacketspersec*payloadSize*8));
  ApplicationContainer serverApp = server.Install (wifirightNodes.Get (sender));
  serverApp.Start (startTime);
  serverApp.Stop(stopTime);


  
  PacketSinkHelper mysinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = mysinkHelper.Install (wifileftNodes.Get (receiver));
  Ptr<PacketSink> temp = StaticCast<PacketSink> (sinkApps.Get (0));
  sink.push_back(temp);
  lastTotalRx.push_back(0);
  sinkApps.Start (startTime);
  sinkApps.Stop (stopTime);
  


  Simulator::Schedule (Seconds (0.1), &CalculateThroughput, i);


  }



//   if (tracing)
//     {
//       phyLeft.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
//       routerLink.EnablePcapAll ("p2p");
//       phyLeft.EnablePcap ("wireless", apDevices.Get (0));
//       csma.EnablePcap ("sma", csmaDevices.Get (0), true);
//     }


   
     // Flow monitor
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  

//   Simulator::Stop (Seconds (15 + 1));
  Simulator::Stop (stopTime + TimeStep (1));

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
  for(uint32_t i = 0; i < noofFlows; i++)
  {
    averageThroughput.push_back((sink[i]->GetTotalRx () * 8.0)/(1e3 * noofFlows));
  }

  
  flowMonitor->SerializeToXmlFile("modified_owntopo_flow.xml", false, false);
  Simulator::Destroy ();

  
  double total_throughput = 0;
  for(uint32_t i = 0; i < noofFlows; i++)
  {
    std::cout << "\nAverage throughput: (flowpair "<<i <<") "<< averageThroughput[i]<< " Kbit/s" << std::endl;
    total_throughput+=averageThroughput[i];
  }
  std::cout << "\nAverage throughput: " << (total_throughput/noofFlows) << " Kbit/s" << std::endl;
 
  return 0;
}


