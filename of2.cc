#include <iostream>

#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/openflow-module.h"
#include "ns3/log.h"
#include "ns3/point-to-point-layout-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("openflow_ptp_example");

ns3::Time timeout = ns3::Seconds(0);


int main(int argc,char *argv[]){
	LogComponentEnable("openflow_ptp_example",LOG_LEVEL_INFO);
	LogComponentEnable("OpenFlowInterface",LOG_LEVEL_INFO);
	LogComponentEnable("OpenFlowSwitchNetDevice",LOG_LEVEL_INFO);

	NS_LOG_INFO("create node");
	NodeContainer terminals;
	terminals.Create(4);
	NodeContainer ptpswitch;
	ptpswitch.Create(1);

	PointToPointHelper ptp;
//	ptp.SetDeviceAttribute("DataRate",StringValue("10mbps"));
	ptp.SetChannelAttribute("Delay",StringValue("1ms"));
	
	NetDeviceContainer terminalDevice;
	NetDeviceContainer switchDevice;

	for(int i=0; i<4; ++i){
		NetDeviceContainer link = ptp.Install(NodeContainer(terminals.Get(i),ptpswitch.Get(0)));
		terminalDevice.Add(link.Get(0));
		switchDevice.Add(link.Get(1));
	}

	NS_LOG_INFO("CREATE SWITCH NODE");
	Ptr<Node> switchNode = ptpswitch.Get(0);
	OpenFlowSwitchHelper swtch;

	NS_LOG_INFO("learing controller");

	Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController>();
	if(!timeout.IsZero()) controller->SetAttribute("ExpirationTime",TimeValue(timeout));
	swtch.Install(switchNode,switchDevice,controller);

	NS_LOG_INFO("learing controller finish");

	InternetStackHelper stack;
	stack.Install(terminals);

	NS_LOG_INFO("create ip");
	Ipv4AddressHelper ipv4;
	ipv4.SetBase("10.1.1.0","255.255.255.0");
	ipv4.Assign(terminalDevice);
	
	NS_LOG_INFO("create application");
//	uint16_t port = 9;
	OnOffHelper onoff("ns3::UdpSocketFactory",Address());
	onoff.SetConstantRate(DataRate("500kb/s"));

	ApplicationContainer app = onoff.Install(terminals.Get(0));
	app.Start(Seconds(1.0));
	app.Stop(Seconds(10.0));

	PacketSinkHelper sink("ns3::UdpSocketFactory",Address());
	ApplicationContainer app1 = sink.Install(terminals.Get(1));
	app1.Start(Seconds(1.0));

	AsciiTraceHelper ascii;
	ptp.EnableAsciiAll(ascii.CreateFileStream("OpenFlow_ptp.tr"));

	NS_LOG_INFO("start simulat");
	Simulator::Run();
	Simulator::Destroy();

}
