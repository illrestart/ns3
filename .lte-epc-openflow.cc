#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/netanim-module.h"


NS_LOG_COMPONENT_DEFINE("EpcExample");

int main(int argc, char *argv[]){
	uint16_t numberofNodes = 2;
	double simTime = 5.0;
	double distance =  60.0;
	double interPacketInterval = 100;

	std::string AnimFile = "lte-epc-openflow.xml";

	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
	Ptr<EpcHelper> epcHelper = CreateObject<epcHelper> ();
	lteHelper
}
