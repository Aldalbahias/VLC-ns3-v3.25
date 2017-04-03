#include "ns3/vlc-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("vlcNetDevice");

NS_OBJECT_ENSURE_REGISTERED (VlcNetDevice);

ns3::TypeId VlcNetDevice::GetTypeId(void)// returns meta-information about VLC_ErrorModel class
		{ 	// including parent class, group name, constructor, and attributes
	static ns3::TypeId tid = ns3::TypeId("VlcNetDevice").SetParent<
			ns3::PointToPointNetDevice>().AddConstructor<VlcNetDevice>();
	return tid;
}

VlcNetDevice::VlcNetDevice() { //: m_txMachineState(READY), m_channel(0), m_linkUp(false), m_currentPkt(0){
	m_mobilityModel = ns3::CreateObject<VlcMobilityModel>();
	m_mobilityModel->SetAzimuth(0);
	m_mobilityModel->SetElevation(0);
	m_mobilityModel->SetPosition(ns3::Vector(0.0, 0.0, 0.0));
}

double VlcNetDevice::GetAzmuth() {
	return this->m_mobilityModel->GetAzimuth();
}

void VlcNetDevice::SetAzmuth(double az) {
	this->m_mobilityModel->SetAzimuth(az);
}

ns3::Vector VlcNetDevice::GetPosition() {
	return this->m_mobilityModel->GetPosition();
}

void VlcNetDevice::SetPosition(ns3::Vector position) {
	m_mobilityModel->SetPosition(position);
}

double VlcNetDevice::GetElevation() {
	return m_mobilityModel->GetElevation();
}

void VlcNetDevice::SetElevation(double elevation) {

	m_mobilityModel->SetElevation(elevation);
}

ns3::Ptr<VlcMobilityModel> VlcNetDevice::GetMobilityModel() {
	return m_mobilityModel;
}
void VlcNetDevice::SetMobilityModel(ns3::Ptr<VlcMobilityModel> model) {
	m_mobilityModel = model;
}

VlcNetDevice::~VlcNetDevice() {

}

void VlcNetDevice::Receive(Ptr<Packet> p) {
	return;
}

bool VlcNetDevice::Send(Ptr<Packet> packet, const Address &dest,
		uint16_t protocolNumber) {
	return PointToPointNetDevice::Send(packet, dest, protocolNumber);
}

/////////////////////////////////////////////////////////////////

/*


 void VlcNetDevice::AddHeader(Ptr<Packet> p, uint16_t protocolNumber) {
 NS_LOG_FUNCTION(this << p << protocolNumber);
 VLCHeader VLC;
 VLC.SetProtocol(EtherToVLC(protocolNumber));
 p->AddHeader(VLC);
 }

 bool VlcNetDevice::ProcessHeader(Ptr<Packet> p, uint16_t& param) {
 NS_LOG_FUNCTION(this << p << param);
 VLCHeader VLC;
 p->RemoveHeader(VLC);
 param = VLCToEther(VLC.GetProtocol());
 return true;
 }

 void VlcNetDevice::DoDispose() {
 NS_LOG_FUNCTION(this);
 m_node = 0;
 m_channel = 0;
 m_receiveErrorModel = 0;
 m_currentPkt = 0;
 NetDevice::DoDispose();
 }

 void VlcNetDevice::SetDataRate(DataRate bps) {
 NS_LOG_FUNCTION(this);
 m_bps = bps;
 }

 void VlcNetDevice::SetInterframeGap(Time t) {
 NS_LOG_FUNCTION(this << t.GetSeconds());
 m_tInterframeGap = t;
 }


 bool VlcNetDevice::TransmitStart(Ptr<Packet> p) {
 NS_LOG_FUNCTION(this << p);
 NS_LOG_LOGIC("UID is " << p->GetUid() << ")");

 //
 // This function is called to start the process of transmitting a packet.
 // We need to tell the channel that we've started wiggling the wire and
 // schedule an event that will be executed when the transmission is complete.
 //
 NS_ASSERT_MSG(m_txMachineState == READY, "Must be READY to transmit");
 m_txMachineState = BUSY;
 m_currentPkt = p;
 m_phyTxBeginTrace (m_currentPkt);

 Time txTime = Time(m_bps.CalculateTxTime(p->GetSize()));
 Time txCompleteTime = txTime + m_tInterframeGap;

 NS_LOG_LOGIC(
 "Schedule TransmitCompleteEvent in " << txCompleteTime.GetSeconds()
 << "sec");
 Simulator::Schedule(txCompleteTime, &VlcNetDevice::TransmitComplete,
 this);

 bool result = m_channel->TransmitStart(p, this, txTime);
 if (result == false) {
 m_phyTxDropTrace(p);
 }
 return result;
 return true;
 }



 void VlcNetDevice::TransmitComplete(void) {
 NS_LOG_FUNCTION(this);

 //
 // This function is called to when we're all done transmitting a packet.
 // We try and pull another packet off of the transmit queue.  If the queue
 // is empty, we are done, otherwise we need to start transmitting the
 // next packet.
 //
 NS_ASSERT_MSG(m_txMachineState == BUSY, "Must be BUSY if transmitting");
 m_txMachineState = READY;

 NS_ASSERT_MSG(m_currentPkt != 0,
 "VLCNetDevice::TransmitComplete(): m_currentPkt zero");

 m_phyTxEndTrace (m_currentPkt);
 m_currentPkt = 0;

 Ptr < Packet > p = m_queue->Dequeue();
 if (p == 0) {
 //
 // No packet was on the queue, so we just exit.
 //
 return;
 }

 //
 // Got another packet off of the queue, so start the transmit process agin.
 //
 m_snifferTrace (p);
 m_promiscSnifferTrace(p);
 TransmitStart(p);
 }


 bool VlcNetDevice::Attach(Ptr<Channel> ch) {
 NS_LOG_FUNCTION(this << &ch);

 m_channel = ch;

 //m_channel->Attach(this);

 //
 // This device is up whenever it is attached to a channel.  A better plan
 // would be to have the link come up when both devices are attached, but this
 // is not done for now.
 //
 NotifyLinkUp();
 return true;
 }

 void VlcNetDevice::SetQueue(Ptr<Queue> q) {
 NS_LOG_FUNCTION(this << q);
 m_queue = q;
 }

 void VlcNetDevice::SetReceiveErrorModel(Ptr<ErrorModel> em) {
 NS_LOG_FUNCTION(this << em);
 m_receiveErrorModel = em;
 }


 void VlcNetDevice::Receive(Ptr<Packet> packet) {
 NS_LOG_FUNCTION(this << packet);
 uint16_t protocol = 0;

 //double BER = CalculateBER();
 // std::cout << BER << std::endl;

 if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt(packet)) {
 //std::cout<< " Dropped "<<std::endl;
 //
 // If we have an error model and it indicates that it is time to lose a
 // corrupted packet, don't forward this packet up, let it go.
 //
 m_phyRxDropTrace(packet);
 } else {
 //
 // Hit the trace hooks.  All of these hooks are in the same place in this
 // device because it is so simple, but this is not usually the case in
 // more complicated devices.
 //
 m_snifferTrace(packet);
 m_promiscSnifferTrace(packet);
 m_phyRxEndTrace(packet);

 //
 // Trace sinks will expect complete packets, not packets without some of the
 // headers.
 //
 Ptr < Packet > originalPacket = packet->Copy();

 //
 // Strip off the on-off-keying protocol header and forward this packet
 // up the protocol stack.  Since this is a simple on-off-keying link,
 // there is no difference in what the promisc callback sees and what the
 // normal receive callback sees.
 //
 ProcessHeader(packet, protocol);

 if (!m_promiscCallback.IsNull()) {
 m_macPromiscRxTrace (originalPacket);
 m_promiscCallback(this, packet, protocol, GetRemote(), GetAddress(),
 NetDevice::PACKET_HOST);
 }

 m_macRxTrace (originalPacket);
 m_rxCallback(this, packet, protocol, GetRemote());
 }
 }

 Ptr<Queue> VlcNetDevice::GetQueue(void) const {
 NS_LOG_FUNCTION(this);
 return m_queue;
 }


 void VlcNetDevice::NotifyLinkUp(void) {
 NS_LOG_FUNCTION(this);
 m_linkUp = true;
 m_linkChangeCallbacks();
 }



 void VlcNetDevice::SetIfIndex(const uint32_t index) {
 NS_LOG_FUNCTION(this);
 m_ifIndex = index;
 }


 uint32_t VlcNetDevice::GetIfIndex(void) const {
 return m_ifIndex;
 }

 Ptr<Channel> VlcNetDevice::GetChannel(void) const {
 return m_channel;
 }

 //
 // This is a on-off-keying device, so we really don't need any kind of address
 // information.  However, the base class NetDevice wants us to define the
 // methods to get and set the address.  Rather than be rude and assert, we let
 // clients get and set the address, but simply ignore them.

 void VlcNetDevice::SetAddress(Address address) {
 NS_LOG_FUNCTION(this << address);
 m_address = Mac48Address::ConvertFrom(address);
 }

 Address VlcNetDevice::GetAddress(void) const {
 return m_address;
 }

 bool VlcNetDevice::IsLinkUp(void) const {
 NS_LOG_FUNCTION(this);
 return m_linkUp;
 }

 void VlcNetDevice::AddLinkChangeCallback(Callback<void> callback) {
 NS_LOG_FUNCTION(this);
 m_linkChangeCallbacks.ConnectWithoutContext(callback);
 }

 //
 // This is a on-off-keying device, so every transmission is a broadcast to
 // all of the devices on the network.
 //
 bool VlcNetDevice::IsBroadcast(void) const {
 NS_LOG_FUNCTION(this);
 return true;
 }

 //
 // We don't really need any addressing information since this is a
 // on-off-keying device.  The base class NetDevice wants us to return a
 // broadcast address, so we make up something reasonable.
 //
 Address VlcNetDevice::GetBroadcast(void) const {
 NS_LOG_FUNCTION(this);
 return Mac48Address("ff:ff:ff:ff:ff:ff");
 }

 bool VlcNetDevice::IsMulticast(void) const {
 NS_LOG_FUNCTION(this);
 return true;
 }

 Address VlcNetDevice::GetMulticast(Ipv4Address multicastGroup) const {
 NS_LOG_FUNCTION(this);
 return Mac48Address("01:00:5e:00:00:00");
 }

 Address VlcNetDevice::GetMulticast(Ipv6Address addr) const {
 NS_LOG_FUNCTION(this << addr);
 return Mac48Address("33:33:00:00:00:00");
 }

 bool VlcNetDevice::IsPointToPoint(void) const {
 NS_LOG_FUNCTION(this);
 return true;
 }

 bool VlcNetDevice::IsBridge(void) const {
 NS_LOG_FUNCTION(this);
 return false;
 }

 bool VlcNetDevice::Send(Ptr<Packet> packet, const Address &dest,
 uint16_t protocolNumber) {
 NS_LOG_FUNCTION(this << packet << dest << protocolNumber);
 NS_LOG_LOGIC("p=" << packet << ", dest=" << &dest);
 NS_LOG_LOGIC("UID is " << packet->GetUid());

 //
 // If IsLinkUp() is false it means there is no channel to send any packet
 // over so we just hit the drop trace on the packet and return an error.
 //
 if (IsLinkUp() == false) {
 m_macTxDropTrace(packet);
 return false;
 }

 //
 // Stick a on-off-keying protocol header on the packet in preparation for
 // shoving it out the door.
 //
 AddHeader(packet, protocolNumber);

 m_macTxTrace(packet);

 //
 // We should enqueue and dequeue the packet to hit the tracing hooks.
 //
 if (m_queue->Enqueue(packet)) {
 //
 // If the channel is ready for transition we send the packet right now
 //
 if (m_txMachineState == READY) {
 packet = m_queue->Dequeue();
 m_snifferTrace(packet);
 m_promiscSnifferTrace(packet);
 return TransmitStart(packet);
 }
 return true;
 }

 // Enqueue may fail (overflow)
 m_macTxDropTrace(packet);
 return false;
 }

 bool VlcNetDevice::SendFrom(Ptr<Packet> packet, const Address &source,
 const Address &dest, uint16_t protocolNumber) {
 NS_LOG_FUNCTION(this << packet << source << dest << protocolNumber);
 return false;
 }

 Ptr<Node> VlcNetDevice::GetNode(void) const {
 return m_node;
 }

 void VlcNetDevice::SetNode(Ptr<Node> node) {
 NS_LOG_FUNCTION(this);
 m_node = node;
 }

 bool VlcNetDevice::NeedsArp(void) const {
 NS_LOG_FUNCTION(this);
 return false;
 }

 void VlcNetDevice::SetReceiveCallback(NetDevice::ReceiveCallback cb) {
 m_rxCallback = cb;
 }

 void VlcNetDevice::SetPromiscReceiveCallback(
 NetDevice::PromiscReceiveCallback cb) {
 m_promiscCallback = cb;
 }

 bool VlcNetDevice::SupportsSendFrom(void) const {
 NS_LOG_FUNCTION(this);
 return false;
 }


 void VlcNetDevice::DoMpiReceive(Ptr<Packet> p) {
 NS_LOG_FUNCTION(this << p);
 Receive(p);
 }


 Address VlcNetDevice::GetRemote(void) const {
 NS_LOG_FUNCTION(this);
 NS_ASSERT(m_channel->GetNDevices() == 2);
 for (uint32_t i = 0; i < m_channel->GetNDevices(); ++i) {
 Ptr < NetDevice > tmp = m_channel->GetDevice(i);
 if (tmp != this) {
 return tmp->GetAddress();
 }
 }
 NS_ASSERT(false);
 // quiet compiler.
 return Address();
 }


 bool VlcNetDevice::SetMtu(uint16_t mtu) {
 NS_LOG_FUNCTION(this << mtu);
 m_mtu = mtu;
 return true;
 }


 uint16_t VlcNetDevice::GetMtu(void) const {
 NS_LOG_FUNCTION(this);
 return m_mtu;
 }


 uint16_t VlcNetDevice::VLCToEther(uint16_t proto) {
 NS_LOG_FUNCTION_NOARGS();
 switch (proto) {
 case 0x0021:
 return 0x0800;   //IPv4
 case 0x0057:
 return 0x86DD;   //IPv6
 default:
 NS_ASSERT_MSG(false, "VLC Protocol number not defined!");
 }
 return 0;
 }

 uint16_t VlcNetDevice::EtherToVLC(uint16_t proto) {
 NS_LOG_FUNCTION_NOARGS();
 switch (proto) {
 case 0x0800:
 return 0x0021;   //IPv4
 case 0x86DD:
 return 0x0057;   //IPv6
 default:
 NS_ASSERT_MSG(false, "VLC Protocol number not defined!");
 }
 return 0;
 }


 */

} /* namespace vlc */

