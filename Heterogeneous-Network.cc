#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/internet-module.h>
#include <ns3/lte-module.h>
#include <ns3/config-store-module.h>
#include <ns3/buildings-module.h>
#include <ns3/point-to-point-helper.h>
#include <ns3/applications-module.h>
#include <ns3/log.h>
#include <iomanip>
#include <ios>
#include <string>
#include <vector>
#include "ns3/netanim-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
    
  

  //Helper//
  //LteHelper
  Ptr <LteHelper> lteHelper;
  lteHelper=CreateObject<LteHelper> ();
  //EpcHelper
  Ptr <EpcHelper> epcHelper; 
  epcHelper=CreateObject<EpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  
  //MobilityHelper//
  MobilityHelper mobility;
  //positionAllocator 
  Ptr<ListPositionAllocator> remoteHostPosition;
  Ptr<ListPositionAllocator> pgwPosition;


  //EPC//
  //Creat a signle remotehost 
  Ptr<Node> remoteHost;
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  remoteHost=remoteHostContainer.Get (0);
  //Set mobility of remotehost
  remoteHostPosition=CreateObject<ListPositionAllocator> ();
  remoteHostPosition->Add (Vector(1000, 2000, 0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(remoteHostPosition);
  mobility.Install(remoteHost);
  //Install internet stack into remotehost
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  //Creat a pgw
  Ptr<Node> pgw;
  pgw=epcHelper->GetPgwNode ();
  //Set mobility of pgw
  pgwPosition=CreateObject<ListPositionAllocator> ();
  pgwPosition->Add (Vector(1000, 1500, 0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(pgwPosition);
  mobility.Install(pgw);
 
  //Create the Internet//
  PointToPointHelper p2ph;
  //Set Attribute
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  //Install internetDevices into pgw and remotehost
  NetDeviceContainer internetDevices;  
  internetDevices=p2ph.Install (pgw, remoteHost);
  //Set IP address
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  //Assign IP for remotehost and pgw
  Ipv4InterfaceContainer internetIpIfaces;
  internetIpIfaces=ipv4h.Assign (internetDevices);
  //interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr;
  remoteHostAddr=internetIpIfaces.GetAddress (1);
  //specify routes so that the remote host can reach LTE UEs
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting;
  remoteHostStaticRouting=ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  //Hetergeneous-Network//
  //Create MeNB
  uint32_t nmEnb=7;
  NodeContainer mEnbs;
  mEnbs.Create(nmEnb);
  NetDeviceContainer mEnbDevs;
  //Create PeNB
  //uint32_t npEnb=nmEnb*3*4;
  //NodeContainer pEnbs;
  //pEnbs.Create(npEnb);
  //Create UE
  //uint32_t nUE=nmEnb*3*30;
  //NodeContainer UE;
  //UE.Create(nUE);
  //Set mobility of mEnbs
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (mEnbs);
  //Macro eNBs in 3-sector hex grid
  Ptr<LteHexGridEnbTopologyHelper> lteHexGridEnbTopologyHelper = CreateObject<LteHexGridEnbTopologyHelper> ();
  lteHexGridEnbTopologyHelper->SetLteHelper (lteHelper);
  lteHexGridEnbTopologyHelper->SetAttribute ("InterSiteDistance", DoubleValue (500));
  lteHexGridEnbTopologyHelper->SetAttribute ("MinX", DoubleValue (1000));
  lteHexGridEnbTopologyHelper->SetAttribute ("MinY", DoubleValue (0));
  lteHexGridEnbTopologyHelper->SetAttribute ("GridWidth", UintegerValue (1));
  lteHexGridEnbTopologyHelper->SetAttribute ("SectorOffset", DoubleValue (0.5));
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (46));
  lteHelper->SetEnbAntennaModelType ("ns3::ParabolicAntennaModel");
  lteHelper->SetEnbAntennaModelAttribute ("Beamwidth", DoubleValue (70));
  lteHelper->SetEnbAntennaModelAttribute ("MaxAttenuation",DoubleValue (20.0));
  lteHelper->SetEnbDeviceAttribute ("DlEarfcn", UintegerValue (100));
  lteHelper->SetEnbDeviceAttribute ("UlEarfcn", UintegerValue (18100));
  lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (25));
  lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (25));
  mEnbDevs=lteHexGridEnbTopologyHelper->SetPositionAndInstallEnbDevice (mEnbs);


  




  AnimationInterface anim ("topology.xml");






























}
