#include <iostream>

#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/netanim-module.h"
#include "ns3/openflow-module.h"

using namespace ns3;


int main(int argc, char* argv[]){
	std::string AnimFile = "test1.xml";
	
	NodeContainer switc;
	switc.Create(1);
	NodeContainer nod;
	nod.Create(2);


	CsmaHelper csma;
	csma.SetChannelAttribute("DataRate",DataRateValue(5000000));
	csma.SetChannelAttribute("Delay",TimeValue(MilliSeconds(2)));

	NetDeviceContainer switcDevice;
	NetDeviceContainer csmaDevice;

	OpenFlowSwitchHelper ofswitch;

	NetDeviceContainer link = csma.Install(NodeContainer(switc.Get(0),nod.Get(0)));
	switcDevice.Add(link.Get(0));
	csmaDevice.Add(link.Get(1));

	NetDeviceContainer link1 = csma.Install(NodeContainer(switc.Get(0),nod.Get(1)));
	switcDevice.Add(link1.Get(0));
	csmaDevice.Add(link1.Get(1));

	Ptr<Node> switchNode = switc.Get(0);

	Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController> ();
	ofswitch.Install(switchNode,switcDevice,controller);


	InternetStackHelper internet;
	internet.Install(nod);
	internet.Install(switc);

	ns3::AnimationInterface::SetConstantPosition(switc.Get(0),50,50);
	ns3::AnimationInterface::SetConstantPosition(nod.Get(0),20,50);
	ns3::AnimationInterface::SetConstantPosition(nod.Get(1),80,50);


	Ipv4AddressHelper ipv4;
//	ipv4.SetBase("10.1.1.0","255.255.255.0");
//	ipv4.Assign(switcDevice);
	Ipv4AddressHelper ipv4_csma;
	ipv4_csma.SetBase("10.1.2.0","255.255.255.0");
	ipv4_csma.Assign(csmaDevice);


	OnOffHelper onoff("ns3::UdpSocketFactory",Address());
	onoff.SetAttribute("OnTime",StringValue("ns3::UniformRandomVariable"));
	onoff.SetAttribute("OffTime",StringValue("ns3::UniformRandomVariable"));
	onoff.SetConstantRate(DataRate("500kb/s"));

	char s[2][10]={"10.1.2.2","10.1.2.1"};
	ApplicationContainer app = onoff.Install(nod.Get(0));
	for(uint32_t i=0; i<2; ++i)
	{	
	AddressValue remoteaddress(InetSocketAddress(s[i],1000));
	onoff.SetAttribute("Remote",remoteaddress);
	app.Add(onoff.Install(nod.Get(i)));
	}
/*
	AddressValue remoteaddress1(InetSocketAddress(s[1],1000));
	onoff.SetAttribute("Remote",remoteaddress1);
	app.Add(onoff.Install(nod.Get(1)));
*/
	app.Start(Seconds(1.0));
	app.Stop(Seconds(10.0));

/*
	PacketSinkHelper sink("ns3::UdpSocketFactory",Address());
	ApplicationContainer app1 = sink.Install(nod.Get(1));
	app1.Start(Seconds(1.0));
*/
		


	ns3::AnimationInterface Anim(AnimFile);
	Simulator::Run();
	Simulator::Destroy();

}
