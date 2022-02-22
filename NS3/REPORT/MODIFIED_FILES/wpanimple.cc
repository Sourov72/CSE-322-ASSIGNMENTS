/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 Universita' di Firenze, Italy
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
 * Author: Tommaso Pecorella <tommaso.pecorella@unifi.it>
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/csma-module.h"


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

//for flow monitor
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

using namespace std;
using namespace ns3;


vector<Ptr<PacketSink>> sink;
vector<uint64_t> lastTotalRx;                     /* The value of the last total received bytes */
uint32_t nwsnleftNodes = 9;
uint32_t nwsnrightNodes = 9;
std::ofstream Myfile("wpanttopology.txt");

void
CalculateThroughput (uint32_t i)
{
  
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink[i]->GetTotalRx () - lastTotalRx[i]) * (double) 8;     /* Convert Application RX Packets to MBits. */
  std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  if(i == 0)
      Myfile << now.GetSeconds() << "\t";
  Myfile << cur << "\t";
  if(i == nwsnleftNodes - 1)
    Myfile << "\n";
  lastTotalRx[i] = sink[i]->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (1), &CalculateThroughput, i);
}

int main (int argc, char** argv)
{
  bool verbose = false;
  
  uint32_t payloadSize = 70;                       /* Transport layer payload size in bytes. */
  std::string tcpVariant = "TcpNewReno";             /* TCP variant type. */
  std::string link_speed = "1Mbps";
  std::string link_delay = "40ms";
  Time startTime = Seconds(0);
  Time stopTime = Seconds(100); /* Simulation time in seconds. */

  uint32_t noofflows = 10;
  uint32_t noofpacketspersec = 10;
  uint32_t speed = 1;
  Packet::EnablePrinting ();


  srand(time(0));

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue("nwsnleftNodes", "number of nwsnleftNodes", nwsnleftNodes);
  cmd.AddValue("nwsnrightNodes", "number of nwsnrightNodes", nwsnrightNodes);
  cmd.AddValue("nwsnrightNodes", "number of nwsnrightNodes", nwsnrightNodes);
  cmd.AddValue("noofflow", "number of flows", noofflows);
  cmd.AddValue("noofpacketspersec", "number of packets per second", noofpacketspersec);
  cmd.AddValue("stopTime", "stop time of simulation", stopTime);
  cmd.AddValue("speed", "the speed of wifi nodes", speed);

  cmd.Parse (argc, argv);


  // cout << noofflows << endl;
  // cout << nwsnleftNodes << endl;
  cout << noofpacketspersec << endl;

  tcpVariant = std::string ("ns3::") + tcpVariant;
  if (verbose)
    {
      LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanMac", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanPhy", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanNetDevice", LOG_LEVEL_ALL);
      LogComponentEnable ("SixLowPanNetDevice", LOG_LEVEL_ALL);
    }

  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (4194304));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (6291456));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));
  

  NodeContainer routers;
  routers.Create(2);


  NodeContainer wsnleftNodes;
  wsnleftNodes.Add(routers.Get(0));
  wsnleftNodes.Create (nwsnleftNodes);


  NodeContainer wsnrightNodes;
  wsnrightNodes.Add(routers.Get(1));
  wsnrightNodes.Create (nwsnrightNodes);

  PointToPointHelper routerLink;
  routerLink.SetDeviceAttribute ("DataRate", StringValue (link_speed));
  routerLink.SetChannelAttribute ("Delay", StringValue (link_delay));


  // for error model
  // Ptr<LrWpanErrorModel> lrWpanError = CreateObject<LrWpanErrorModel> ();
  // for( uint32_t i=0; i<( nwsnleftNodes + nwsnrightNodes + 2); i++ ) {
  //   std::string path = "/NodeList/";
  //   path += std::to_string(i);
  //   path += "/DeviceList/0/$ns3::LrWpanNetDevice/Phy/$ns3::LrWpanPhy/ErrorModel";
  //   Config::Set(path, PointerValue(lrWpanError));
  // }


  NetDeviceContainer r2rDevices;
  r2rDevices = routerLink.Install (routers);

  MobilityHelper mobilityLeft;
  mobilityLeft.SetPositionAllocator("ns3::RandomRectanglePositionAllocator");


  mobilityLeft.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                            "Mode", StringValue ("Time"),
                            "Time", StringValue ("2s"),
                            "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=5.0]"),
                            "Bounds", RectangleValue (Rectangle (-1, 1, -1, 1)));

  mobilityLeft.Install (wsnleftNodes);

  LrWpanHelper lrWpanHelperleft;
  // Add and install the LrWpanNetDevice for each node
  NetDeviceContainer lrwpanDevicesleft = lrWpanHelperleft.Install (wsnleftNodes);

  // Fake PAN association and short address assignment.
  // This is needed because the lr-wpan module does not provide (yet)
  // a full PAN association procedure.
  lrWpanHelperleft.AssociateToPan (lrwpanDevicesleft, 0);


  ///// for right side

  MobilityHelper mobilityright;


  mobilityright.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                            "Mode", StringValue ("Time"),
                            "Time", StringValue ("2s"),
                            "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=5.0]"),
                            "Bounds", RectangleValue (Rectangle (-1, 1, -1, 1)));

  mobilityright.Install (wsnrightNodes);

  LrWpanHelper lrWpanHelperright;
  // Add and install the LrWpanNetDevice for each node
  NetDeviceContainer lrwpanDevicesright = lrWpanHelperright.Install (wsnrightNodes);

  // Fake PAN association and short address assignment.
  // This is needed because the lr-wpan module does not provide (yet)
  // a full PAN association procedure.
  lrWpanHelperright.AssociateToPan (lrwpanDevicesright, 0);


  InternetStackHelper internetv6;
  internetv6.Install (wsnleftNodes);
  internetv6.Install (wsnrightNodes);


  SixLowPanHelper sixLowPanHelperright;
  NetDeviceContainer sixLowPanDevicesright = sixLowPanHelperright.Install (lrwpanDevicesright);

  SixLowPanHelper sixLowPanHelperleft;
  NetDeviceContainer sixLowPanDevicesleft = sixLowPanHelperleft.Install (lrwpanDevicesleft);


  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wsnleftdevicesInterfaces;
  wsnleftdevicesInterfaces = ipv6.Assign (sixLowPanDevicesleft);
  wsnleftdevicesInterfaces.SetForwarding (0, true);
  wsnleftdevicesInterfaces.SetDefaultRouteInAllNodes (0);

  ipv6.SetBase (Ipv6Address ("2001:2::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wsnrightdevicesInterfaces;
  wsnrightdevicesInterfaces = ipv6.Assign (sixLowPanDevicesright);
  wsnrightdevicesInterfaces.SetForwarding (0, true);
  wsnrightdevicesInterfaces.SetDefaultRouteInAllNodes (0);

  // wsnrightdevicesInterfaces.SetForwarding (1, true);
  // wsnrightdevicesInterfaces.SetDefaultRouteInAllNodes (1);

  ipv6.SetBase (Ipv6Address ("2001:3::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer routerInterfaces;
  routerInterfaces = ipv6.Assign (r2rDevices);
  routerInterfaces.SetForwarding(0, true);
  routerInterfaces.SetDefaultRouteInAllNodes (0);
  routerInterfaces.SetForwarding(1, true);
  routerInterfaces.SetDefaultRouteInAllNodes (1);


  for (uint32_t i = 0; i < sixLowPanDevicesleft.GetN (); i++)
  {
    Ptr<NetDevice> dev = sixLowPanDevicesleft.Get (i);
    dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
    dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
  }


  for (uint32_t i = 0; i < sixLowPanDevicesright.GetN (); i++)
  {
    Ptr<NetDevice> dev = sixLowPanDevicesright.Get (i);
    dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
    dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
  }

  // uint32_t mtu_bytes = 180;
  // Header* temp_header = new Ipv6Header ();
  // uint32_t ip_header = temp_header->GetSerializedSize ();
  // delete temp_header;
  // temp_header = new TcpHeader ();
  // uint32_t tcp_header = temp_header->GetSerializedSize ();
  // delete temp_header;
  // uint32_t tcp_adu_size = mtu_bytes - 20 - (ip_header + tcp_header);
  // payloadSize = tcp_adu_size;
  // cout<<"adu "<<tcp_adu_size<<endl;

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));



  uint16_t port = 5000;

  for(uint32_t i = 0; i < noofflows; i++){

    
    int sender = (rand() % (nwsnleftNodes - 1));
    int receiver = sender;

    port +=1 ;
  //   BulkSendHelper server ("ns3::TcpSocketFactory", Inet6SocketAddress (wsnleftdevicesInterfaces.GetAddress (i, 1), port));
  //   server.SetAttribute ("MaxBytes", UintegerValue (0));
  //   server.SetAttribute ("SendSize", UintegerValue (120));
  //   ApplicationContainer serverApp = server.Install (wsnrightNodes.Get (i + 2));
  //   serverApp.Start (startTime);
  //     //under this commented line is not needed except schedule
  // //   Simulator::Schedule (Seconds (0.2), &TraceCwnd, 0, 0);
  //   serverApp.Stop (stopTime);

    OnOffHelper server ("ns3::TcpSocketFactory", Inet6SocketAddress (wsnleftdevicesInterfaces.GetAddress (receiver, 1), port));

    server.SetAttribute ("PacketSize", UintegerValue (payloadSize));
    server.SetAttribute ("MaxBytes", UintegerValue (0));
    server.SetConstantRate (DataRate (noofpacketspersec*payloadSize*8));

    ApplicationContainer serverApp = server.Install (wsnrightNodes.Get (sender));
    serverApp.Start (startTime);
    serverApp.Stop (stopTime);

    PacketSinkHelper mysinkHelper ("ns3::TcpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), port));
    ApplicationContainer sinkApps = mysinkHelper.Install (wsnleftNodes.Get (receiver));
    Ptr<PacketSink> temp = StaticCast<PacketSink> (sinkApps.Get (0));
    sink.push_back(temp);
    lastTotalRx.push_back(0);
    sinkApps.Start (startTime);
    sinkApps.Stop (stopTime);
        
    // Simulator::Schedule (Seconds (0.001), &CalculateThroughput, i);


    }

  // AsciiTraceHelper ascii;
  // lrWpanHelper.EnableAsciiAll (ascii.CreateFileStream ("Ping-6LoW-lr-wpan-meshunder-lr-wpan.tr"));
  // lrWpanHelper.EnablePcapAll (std::string ("Ping-6LoW-lr-wpan-meshunder-lr-wpan"), true);

  // csmaHelper.EnableAsciiAll (ascii.CreateFileStream ("Ping-6LoW-lr-wpan-meshunder-csma.tr"));
  // csmaHelper.EnablePcapAll (std::string ("Ping-6LoW-lr-wpan-meshunder-csma"), true);

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();


  Simulator::Stop (stopTime + TimeStep (1));

  Simulator::Run ();
  // std::vector<double> averageThroughput;
  // for(uint32_t i = 0; i < nSourceLeftNodes; i++)
  // {
  //   averageThroughput.push_back((sink[i]->GetTotalRx () * 8.0)/(1e0 * 10.0));
  // }

  
  flowMonitor->SerializeToXmlFile("wpanflow.xml", false, false);
  Simulator::Destroy ();

  
  // double total_throughput = 0;
  // for(uint32_t i = 0; i < nSourceLeftNodes; i++)
  // {
  //   std::cout << "\nAverage throughput: (flowpair "<<i <<") "<< averageThroughput[i]<< " Mbit/s" << std::endl;
  //   total_throughput+=averageThroughput[i];
  // }
  // std::cout << "\nAverage throughput: " << (total_throughput/nwsnleftNodes) << " Kbit/s" << std::endl;

  return 0;

}

