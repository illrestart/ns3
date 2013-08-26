#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/openflow-module.h"
#include "ns3/log.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"



using namespace ns3;

NS_LOG_COMPONENT_DEFINE("OpenFlowCsmaSwitchExample");

ns3::Time timeout = ns3::Seconds(0);

bool SetTimeout(std::string value)
{
	try{
		timeout = ns3::Seconds(atof (value.c_str()));
		return true;
	}
	catch(...){
		return false;
	}
	return false;
}

int main(int argc,char* argv[]){
	LogComponentEnable("OpenFlowCsmaSwitchExample",LOG_LEVEL_INFO);
	LogComponentEnable("OpenFlowInterface",LOG_LEVEL_INFO);
	LogComponentEnable("OpenFlowSwitchNetDevice",LOG_LEVEL_INFO);
	

	std::string AnimFile = "of1.xml";


	NS_LOG_INFO("Create nodes.");
	NodeContainer terminals;
	terminals.Create(4);

	NodeContainer csmaSwitch;
	csmaSwitch.Create(1);

	NS_LOG_INFO("Build Topolgy.");
	CsmaHelper csma;
	csma.SetChannelAttribute("DataRate",DataRateValue(5000000));
	csma.SetChannelAttribute("Delay",TimeValue(MilliSeconds(2)));

//create device container for nodes

	NetDeviceContainer terminalDevices;
	NetDeviceContainer switchDevices;
	for(int i =0 ;i<4 ; i++){
		NetDeviceContainer link = csma.Install (NodeContainer(terminals.Get(i),csmaSwitch));
		terminalDevices.Add(link.Get(0));
		switchDevices.Add(link.Get(1));
	}

	Ptr<Node> switchNode = csmaSwitch.Get(0);
	OpenFlowSwitchHelper swtch;

	Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController> ();
	if(!timeout.IsZero()) controller->SetAttribute("ExpirationTime",TimeValue(timeout));
	swtch.Install (switchNode,switchDevices,controller);	

	//mobility
//	MobilityHelper mobility;
//	mobility.SetMobilityModel("ns3::RandomWalk2MobilityModel","Bounds",RectangleValue(Rectangle(-50,50,-25,50)));
	
	ns3::AnimationInterface::SetConstantPosition(csmaSwitch.Get(0),50,50);
	ns3::AnimationInterface::SetConstantPosition(terminals.Get(0),40,60);
	ns3::AnimationInterface::SetConstantPosition(terminals.Get(1),60,60);
	ns3::AnimationInterface::SetConstantPosition(terminals.Get(2),40,40);
	ns3::AnimationInterface::SetConstantPosition(terminals.Get(3),60,40);


	InternetStackHelper internet;
	internet.Install(terminals);
	

	NS_LOG_INFO ("Assign IP Addresses.");
	Ipv4AddressHelper ipv4;
	ipv4.SetBase("10.1.1.0","255.255.255.0");
	ipv4.Assign(terminalDevices);

	NS_LOG_INFO ("Create Applications.");
//	uint16_t port = 9;
//	OnOffHelper onoff("ns3::UdpSocketFactory",Address(InetSocketAddress(Ipv4Address("10.1.1.2"),port)));
	OnOffHelper onoff("ns3::UdpSocketFactory",Address());
	onoff.SetAttribute("OnTime",StringValue("ns3::UniformRandomVariable"));
	onoff.SetAttribute("OffTime",StringValue("ns3::UniformRandomVariable"));
	onoff.SetConstantRate(DataRate("500kb/s"));

	ApplicationContainer app=onoff.Install(terminals.Get(0));
	char s[4][10]={"10.1.1.0","10.1.1.1","10.1.1.2","10.1.1.3"};
	for(uint32_t j=0; j<4 ; ++j){
	AddressValue RemoteAddress(InetSocketAddress(s[j],1000));
	onoff.SetAttribute("Remote",RemoteAddress);
	app.Add(onoff.Install(terminals.Get(j)));
	}

	app.Start(Seconds(1.0));
	app.Stop(Seconds(10.0));

	
//	PacketSinkHelper sink("ns3::UdpSocketFactory",Address(InetSocketAddress(Ipv4Address::GetAny(),port)));

//	ApplicationContainer app1 = sink.Install(terminals.Get(1));
//	app1.Start(Seconds(0.0));
	
	NS_LOG_INFO("CONFIGURE TRACING");
	
	AsciiTraceHelper ascii;
	csma.EnableAsciiAll (ascii.CreateFileStream("openflow-switch.tr"));

	csma.EnablePcapAll("openflow-switch",false);

	NS_LOG_INFO("Run Simulation.");

	ns3::AnimationInterface::SetNodeDescription(terminals,"UE");
	ns3::AnimationInterface::SetNodeDescription(csmaSwitch,"UGW");

	AnimationInterface Anim(AnimFile);

//	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
	Simulator::Run();
	Simulator::Destroy();

	NS_LOG_INFO("Done.");

	

	}
