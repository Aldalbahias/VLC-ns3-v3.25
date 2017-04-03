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
 *
 */

//
// Network topology
//
//           10Mb/s, 10ms       10Mb/s, 10ms
//       n0-----------------n1-----------------n2
//
//
// - Tracing of queues and packet receptions to file
//   "tcp-large-transfer.tr"
// - pcap traces also generated in the following files
//   "tcp-large-transfer-$n-$i.pcap" where n and i represent node and interface
// numbers respectively
//  Usage (e.g.): ./waf --run tcp-large-transfer
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
//#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/vlc-channel-helper.h"
#include "ns3/vlc-device-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("VisibleLightCommunication");

void PrintPacketData(Ptr<const Packet> p, uint32_t size);

// The number of bytes to send in this simulation.
static const uint32_t totalTxBytes = 80000;
static uint32_t currentTxBytes = 0;

// Perform series of 1040 byte writes (this is a multiple of 26 since
// we want to detect data splicing in the output stream)
static const uint32_t writeSize = 2048;
uint8_t data[writeSize];

// These are for starting the writing process, and handling the sending
// socket's notification upcalls (events).  These two together more or less
// implement a sending "Application", although not a proper ns3::Application
// subclass.

void StartFlow(Ptr<Socket>, Ipv4Address, uint16_t);

void WriteUntilBufferFull(Ptr<Socket>, uint32_t);

std::vector<double> Received(1, 0);
std::vector<double> theTime(1, 0);
//////////////////////////////////////
//Function to generate signals.
std::vector<double>& GenerateSignal(int size, double dutyRatio);

static void RxEnd(Ptr<const Packet> p) { // used for tracing and calculating throughput

	//PrintPacketData(p,p->GetSize());

	Received.push_back(Received.back() + p->GetSize()); // appends on the received packet to the received data up until that packet and adds that total to the end of the vector
	theTime.push_back(Simulator::Now().GetSeconds()); // keeps track of the time during simulation that a packet is received
	//NS_LOG_UNCOND("helooooooooooooooooo RxEnd");
}

static void TxEnd(Ptr<const Packet> p) { // also used as a trace and for calculating throughput

	Received.push_back(Received.back() + p->GetSize()); // same as for the RxEnd trace
	theTime.push_back(Simulator::Now().GetSeconds()); 	//
	//NS_LOG_UNCOND("helooooooooooooooooo TxEnd");
}

static void CwndTracer(uint32_t oldval, uint32_t newval) {
	NS_LOG_INFO("Moving cwnd from " << oldval << " to " << newval);
}

int main(int argc, char *argv[]) {
	// Users may find it convenient to turn on explicit debugging
	// for selected modules; the below lines suggest how to do this
	//  LogComponentEnable("TcpL4Protocol", LOG_LEVEL_ALL);
	//  LogComponentEnable("TcpSocketImpl", LOG_LEVEL_ALL);
	LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
	//  LogComponentEnable("TcpLargeTransfer", LOG_LEVEL_ALL);

	//parameters:
	double PhotoDetectorArea = (1.3e-5); 	// to set the photo dectror area
	double Band_factor_Noise_Signal = (10.0);

	CommandLine cmd;
	cmd.Parse(argc, argv);

	// initialize the tx buffer.
	for (uint32_t i = 0; i < writeSize; ++i) {
		char m = toascii(97 + i % 26);
		data[i] = m;
	}

	for (double dist = 0.1; dist < 2; dist += 0.1) {
		//double dist = 2;

		// Here, we will explicitly create three nodes.  The first container contains
		// nodes 0 and 1 from the diagram above, and the second one contains nodes
		// 1 and 2.  This reflects the channel connectivity, and will be used to
		// install the network interfaces and connect them with a channel.
		NodeContainer n0n1;
		n0n1.Create(2);

		NodeContainer n1n2;
		n1n2.Add(n0n1.Get(1));
		n1n2.Create(1);
		
		MobilityHelper mobility;
		Ptr < ListPositionAllocator > m_listPosition = CreateObject<
				ListPositionAllocator>();
		m_listPosition->Add(Vector(250.0, 500.0, 0.0));
		m_listPosition->Add(Vector(500.0, 500.0, 0.0));
		m_listPosition->Add(Vector(750.0, 500.0, 0.0));
		
		mobility.SetPositionAllocator(m_listPosition);
		mobility.SetMobilityModel("ns3::VlcMobilityModel");

		NodeContainer allNodes(n0n1, n1n2.Get(1));
		mobility.Install(allNodes);

		VlcDeviceHelper devHelperVPPM; //VLC Device Helper to manage the VLC Device and device properties.

		//Creating and setting properties for the first transmitter.
		devHelperVPPM.CreateTransmitter("THE_TRANSMITTER1");
		/****************************************TX-SIGNAL************************************
		 * 1000-SIGNAL SIZE, 0.5-DUTY RATIO, 0-BIAS, 9.25E-5-PEAK VOLTAGE, 0-MINIMUM VOLTAGE
		 ************************************************************************************/
		devHelperVPPM.SetTXSignal("THE_TRANSMITTER1", 1000, 0.5, 0, 9.25e-5, 0);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER1", "Bias", 0);//SETTING BIAS VOLTAGE MOVES THE SIGNAL VALUES BY THE AMOUNT OF BIASING
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER1", "SemiAngle", 35);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER1", "Azimuth", 0);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER1", "Elevation",180.0);
		devHelperVPPM.SetTrasmitterPosition("THE_TRANSMITTER1", 0.0, 0.0, 52.0);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER1", "Gain", 70);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER1", "DataRateInMBPS",0.3);

		//Creating and setting properties for the second transmitter.
		devHelperVPPM.CreateTransmitter("THE_TRANSMITTER2");
		/****************************************TX-SIGNAL************************************
		* 1000-SIGNAL SIZE, 0.5-DUTY RATIO, 0-BIAS, 9.25E-5-PEAK VOLTAGE, 0-MINIMUM VOLTAGE
		************************************************************************************/
		devHelperVPPM.SetTXSignal("THE_TRANSMITTER2", 1000, 0.5, 0, 9.25e-5, 0);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER2", "Bias", 0);//SETTING BIAS VOLTAGE MOVES THE SIGNAL VALUES BY THE AMOUNT OF BIASING
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER2", "SemiAngle", 35);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER2", "Azimuth", 0);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER2", "Elevation",180.0);
		devHelperVPPM.SetTrasmitterPosition("THE_TRANSMITTER2", 0.0, 0.0, 52.0);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER2", "Gain", 70);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER2", "DataRateInMBPS",0.3);


		//Creating and setting properties for the first receiver.
		devHelperVPPM.CreateReceiver("THE_RECEIVER1");
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "FilterGain", 1);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "RefractiveIndex", 1.5);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "FOVAngle", 28.5);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "ConcentrationGain", 0);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "PhotoDetectorArea",1.3e-5);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "RXGain", 0);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "PhotoDetectorArea",PhotoDetectorArea);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "Beta", 1);
		/****************************************MODULATION SCHEME SETTINGS******************
		*AVILABLE MODULATION SCHEMES ARE: [1]. VlcErrorModel::PAM4, [2]. VlcErrorModel::OOK [3]. VlcErrorModel::VPPM
		*AVILABLE MODULATION SCHEMES [4]. VlcErrorModel::PSK4 [5]. VlcErrorModel::PSK16. [6]. VlcErrorModel::QAM4 [7]. VlcErrorModel::QAM16.
		************************************************************************************/
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "SetModulationScheme",VlcErrorModel::PSK4);
		//devHelperVPPM.SetReceiverParameter("THE_RECEIVER1", "DutyCycle", 0.85); //Dutycycle is only valid for VPPM

		//Creating and setting properties for the second receiver.
		devHelperVPPM.CreateReceiver("THE_RECEIVER2");
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "FilterGain", 1);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "RefractiveIndex", 1.5);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "FOVAngle", 28.5);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "ConcentrationGain", 0);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "PhotoDetectorArea",1.3e-5);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "RXGain", 0);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "PhotoDetectorArea",PhotoDetectorArea);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "Beta", 1);
		/****************************************MODULATION SCHEME SETTINGS******************
		*AVILABLE MODULATION SCHEMES ARE: [1]. VlcErrorModel::VPPM, [2]. VlcErrorModel::OOK [3]. VlcErrorModel::PAM
		*AVILABLE MODULATION SCHEMES [4]. VlcErrorModel::PSK4 [5]. VlcErrorModel::PSK16. [6]. VlcErrorModel::QAM4 [7]. VlcErrorModel::QAM16.
		*************************************************************************************/
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "SetModulationScheme",VlcErrorModel::PSK4);
		//devHelperVPPM.SetReceiverParameter("THE_RECEIVER2", "DutyCycle", 0.85);//Dutycycle is only valid for VPPM
		
		VlcChannelHelper chHelper;
		chHelper.CreateChannel("THE_CHANNEL1");
		chHelper.SetPropagationLoss("THE_CHANNEL1", "VlcPropagationLoss");
		chHelper.SetPropagationDelay("THE_CHANNEL1", 2);
		chHelper.AttachTransmitter("THE_CHANNEL1", "THE_TRANSMITTER1",&devHelperVPPM);
		chHelper.AttachReceiver("THE_CHANNEL1", "THE_RECEIVER1", &devHelperVPPM);
		chHelper.SetChannelParameter("THE_CHANNEL1", "TEMP", 295);
		chHelper.SetChannelParameter("THE_CHANNEL1", "BAND_FACTOR_NOISE_SIGNAL",Band_factor_Noise_Signal );
		chHelper.SetChannelWavelength("THE_CHANNEL1", 380, 780);
		chHelper.SetChannelParameter("THE_CHANNEL1", "ElectricNoiseBandWidth",3 * 1e5);
		
		chHelper.CreateChannel("THE_CHANNEL2");
		chHelper.SetPropagationLoss("THE_CHANNEL2", "VlcPropagationLoss");
		chHelper.SetPropagationDelay("THE_CHANNEL2", 2);
		chHelper.AttachTransmitter("THE_CHANNEL2", "THE_TRANSMITTER2",&devHelperVPPM);
		chHelper.AttachReceiver("THE_CHANNEL2", "THE_RECEIVER2", &devHelperVPPM);
		chHelper.SetChannelParameter("THE_CHANNEL2", "TEMP", 295);
		chHelper.SetChannelParameter("THE_CHANNEL2", "BAND_FACTOR_NOISE_SIGNAL",Band_factor_Noise_Signal);
		chHelper.SetChannelWavelength("THE_CHANNEL2", 380, 780);
		chHelper.SetChannelParameter("THE_CHANNEL2", "ElectricNoiseBandWidth",3 * 1e5);
		
		// And then install devices and channels connecting our topology.
		NetDeviceContainer dev0 = chHelper.Install(n0n1.Get(0), n0n1.Get(1),
				&devHelperVPPM, &chHelper, "THE_TRANSMITTER1", "THE_RECEIVER1",
				"THE_CHANNEL1");

		NetDeviceContainer dev1 = chHelper.Install(n1n2.Get(0), n1n2.Get(1),
				&devHelperVPPM, &chHelper, "THE_TRANSMITTER2", "THE_RECEIVER2",
				"THE_CHANNEL2");
		
		// Now add ip/tcp stack to all nodes.
		InternetStackHelper internet;
		internet.InstallAll();
		
		// Later, we add IP addresses.
		Ipv4AddressHelper ipv4;
		ipv4.SetBase("10.1.3.0", "255.255.255.0");
		ipv4.Assign(dev0);
		ipv4.SetBase("10.1.2.0", "255.255.255.0");
		Ipv4InterfaceContainer ipInterfs = ipv4.Assign(dev1);
		
		// and setup ip routing tables to get total ip-level connectivity.
 		Ipv4GlobalRoutingHelper::PopulateRoutingTables();
		
		///////////////////////////////////////////////////////////////////////////
		// Simulation 1
		//
		// Send 2000000 bytes over a connection to server port 50000 at time 0
		// Should observe SYN exchange, a lot of data segments and ACKS, and FIN
		// exchange.  FIN exchange isn't quite compliant with TCP spec (see release
		// notes for more info)
		//
		///////////////////////////////////////////////////////////////////////////
		uint16_t servPort = 4000;
		// Create a packet sink to receive these packets on n2...
		PacketSinkHelper sink("ns3::TcpSocketFactory",
				InetSocketAddress(Ipv4Address::GetAny(), servPort));
		
		ApplicationContainer apps = sink.Install(n1n2.Get(1));
		
		devHelperVPPM.SetTrasmitterPosition("THE_TRANSMITTER1", 0.0, 0.0, 0.0);
		devHelperVPPM.SetTrasmitterPosition("THE_TRANSMITTER2", 0.0, 0.0, 0.0);
		
		devHelperVPPM.SetReceiverPosition("THE_RECEIVER1", 0.0, 0.0, dist);
		devHelperVPPM.SetReceiverPosition("THE_RECEIVER2", 0.0, 0.0, dist);

		apps.Start(Seconds(0.0));
		apps.Stop(Seconds(4.0));
		// Create a source to send packets from n0.  Instead of a full Application
		// and the helper APIs you might see in other example files, this example
		// will use sockets directly and register some socket callbacks as a sending
		// "Application".
		// Create and bind the socket...
		Ptr < Socket > localSocket = Socket::CreateSocket(n0n1.Get(0),
				TcpSocketFactory::GetTypeId());
		localSocket->Bind();

		// Trace changes to the congestion window
		Config::ConnectWithoutContext(
				"/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
				MakeCallback(&CwndTracer));
		dev1.Get(1)->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&RxEnd)); //traces to allow us to see what and when data is sent through the network
		dev1.Get(1)->TraceConnectWithoutContext("PhyTxEnd", MakeCallback(&TxEnd)); //traces to allow us to see what and when data is received through the network

		// ...and schedule the sending "Application"; This is similar to what an
		// ns3::Application subclass would do internally.
		Simulator::ScheduleNow(&StartFlow, localSocket, ipInterfs.GetAddress(1),
				servPort);

		// One can toggle the comment for the following line on or off to see the
		// effects of finite send buffer modelling.  One can also change the size of
		// said buffer.
		//localSocket->SetAttribute("SndBufSize", UintegerValue(4096));
		//Ask for ASCII and pcap traces of network traffic
		AsciiTraceHelper ascii;
		// p2p.EnableAsciiAll (ascii.CreateFileStream ("tcp-large-transfer.tr"));
		//p2p.EnablePcapAll ("tcp-large-transfer");

		// Finally, set up the simulator to run.  The 1000 second hard limit is a
		// failsafe in case some change above causes the simulation to never end
		AnimationInterface anim("visible-light-communication.xml");

		Simulator::Stop(Seconds(5.0));
		Simulator::Run();

		double throughput = ((Received.back() * 8)) / theTime.back(); //goodput calculation
		std::cout << "throughput value is" << throughput << std::endl;

		Ptr < VlcRxNetDevice > rxHandle = devHelperVPPM.GetReceiver(
				"THE_RECEIVER1");
		double goodput = rxHandle->ComputeGoodPut();
	       std::cout<< "Good Packet Received Size is  "<< goodput<< std::endl;
		goodput *= 8;
		goodput /= theTime.back();
		goodput /= 1024;
		std::cout << "Simulation time is "<< theTime.back()<<std::endl;
		std::cout << "goodput value is " << goodput << std::endl;
		//std::cout << chHelper.GetChannelSNR("THE_CHANNEL2") << "\t" << std::endl;

		//NS_LOG_UNCOND(throughput);

		//std::cout<<throughput<<std::endl;

		Received.clear();

		Simulator::Destroy();
		currentTxBytes = 0;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//begin implementation of sending "Application"
void StartFlow(Ptr<Socket> localSocket, Ipv4Address servAddress,
		uint16_t servPort) {
	//NS_LOG_UNCOND("helooooooooooooooooo StartFlow");
	localSocket->Connect(InetSocketAddress(servAddress, servPort)); //connect

	// tell the tcp implementation to call WriteUntilBufferFull again
	// if we blocked and new tx buffer space becomes available
	localSocket->SetSendCallback(MakeCallback(&WriteUntilBufferFull));
	WriteUntilBufferFull(localSocket, localSocket->GetTxAvailable());
}

void WriteUntilBufferFull(Ptr<Socket> localSocket, uint32_t txSpace) {
	//NS_LOG_UNCOND("helooooooooooooooooo WriteUntilBufferFull");
	while (currentTxBytes < totalTxBytes && localSocket->GetTxAvailable() > 0) {
		
		uint32_t left = totalTxBytes - currentTxBytes;
		uint32_t dataOffset = currentTxBytes % writeSize;
		uint32_t toWrite = writeSize - dataOffset;
		toWrite = std::min (toWrite, left);
		toWrite = std::min (toWrite, localSocket->GetTxAvailable ());
		
		Ptr<Packet> p = Create<Packet>(&data[dataOffset], toWrite);
		Ptr<Node> startingNode = localSocket->GetNode();
		Ptr<VlcTxNetDevice> txOne = DynamicCast<VlcTxNetDevice>(startingNode->GetDevice(0) );
		txOne->EnqueueDataPacket(p);
      
		int amountSent = localSocket->Send (&data[dataOffset], toWrite, 0);
		if(amountSent < 0)
		{
			// we will be called again when new tx space becomes available.
			return;
		}
		
		currentTxBytes += amountSent;
	}
	
	localSocket->Close();
}

std::vector<double>& GenerateSignal(int size, double dutyRatio) {
	std::vector<double> *result = new std::vector<double>();
	result->reserve(size);

	double bias = 0;
	double Vmax = 4.5;
	double Vmin = 0.5;

	for (int i = 0; i < size; i++) {
		if (i < size * dutyRatio) {
			result->push_back(Vmax + bias);
		} else {
			result->push_back(Vmin + bias);
		}
	}

	return *result;
}

void PrintPacketData(Ptr<const Packet> p, uint32_t size) {
	uint8_t *data = new uint8_t[size];

	p->CopyData(data, size);

	for (uint32_t i = 0; i < size; i++) {
		std::cout << (int) data[i] << " ";
	}

	std::cout << std::endl;

	delete[] data;

}

