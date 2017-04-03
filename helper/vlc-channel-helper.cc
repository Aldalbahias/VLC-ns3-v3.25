#include "vlc-channel-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("VLCCHANNELHELPER");

VlcChannelHelper::VlcChannelHelper() {
	//NS_LOG_FUNCTION(this);
	m_queueFactory.SetTypeId("ns3::DropTailQueue");
	m_channelFactory.SetTypeId("VlcChannel");
	m_remoteChannelFactory.SetTypeId("ns3::PointToPointRemoteChannel");
}

void VlcChannelHelper::CreateChannel(std::string channelName) {
	//NS_LOG_FUNCTION(this);
	this->m_channel[channelName] = CreateObject<VlcChannel>();
}

void VlcChannelHelper::SetChannelParameter(std::string chName,
		std::string paramName, double value) {
	if (paramName == "TEMP") {
		this->m_channel[chName]->SetTemperature(value);
	} else if (paramName == "ElectricNoiseBandWidth") {
		this->m_channel[chName]->SetElectricNoiseBandWidth(value);
	}
}

void VlcChannelHelper::SetPropagationLoss(std::string channelName,
		std::string propagationLossType) {
	//NS_LOG_FUNCTION(this);
	if (propagationLossType == "VlcPropagationLoss") {
		this->m_channel[channelName]->SetPropagationLossModel(
				CreateObject<VlcPropagationLossModel>());
	}
}

void VlcChannelHelper::SetPropagationDelay(std::string channelName,
		double value) {
	//NS_LOG_FUNCTION(this);
	this->m_channel[channelName]->SetPropagationDelay(value);

}

double VlcChannelHelper::GetChannelSNR(std::string chName) {
	ns3::Ptr < VlcRxNetDevice > rx = DynamicCast < VlcRxNetDevice
			> (m_channel[chName]->GetDevice(1));
	this->m_channel[chName]->DoCalcPropagationLossForSignal(0);
	this->m_channel[chName]->CalculateNoiseVar();
	double snr = this->m_channel[chName]->GetSNR();
	rx->SetSNRForErrorModel(snr);
	return snr;
}

void VlcChannelHelper::SetChannelWavelength(std::string channelName, int lower,
		int upper) {
	this->m_channel[channelName]->SetWavelength(lower, upper);
}

void VlcChannelHelper::AttachTransmitter(std::string chName,
		std::string TXDevName, ns3::Ptr<VlcDeviceHelper> devHelper) {
	//NS_LOG_FUNCTION(this);
	//this->m_channel[chName]->Attach(devHelper->GetTransmitter(TXDevName));
	ns3::Ptr < VlcPropagationLossModel > loss = ns3::DynamicCast
			< VlcPropagationLossModel
			> (m_channel[chName]->GetPropagationLossModel());
	loss->SetTxPowerMAX(devHelper->GetTransmitter(TXDevName)->GetTXPowerMax());
	loss->SetTXGain(devHelper->GetTransmitter(TXDevName)->GetTXGain());

	//this->m_channel[chName]->SetPropagationLossParametersFromTX(devHelper->GetTransmitter(TXDevName));
}

void VlcChannelHelper::AttachReceiver(std::string chName, std::string RXDevName,
		ns3::Ptr<VlcDeviceHelper> devHelper) {
	//NS_LOG_FUNCTION(this);
	//this->m_channel[chName]->Attach(devHelper->GetReceiver(RXDevName));

	ns3::Ptr < VlcPropagationLossModel > loss = ns3::DynamicCast
			< VlcPropagationLossModel
			> (m_channel[chName]->GetPropagationLossModel());

	loss->SetFilterGain(devHelper->GetReceiver(RXDevName)->GetFilterGain());

	loss->SetConcentratorGain(
			devHelper->GetReceiver(RXDevName)->GetConcentrationGain());

	loss->SetRXGain(devHelper->GetReceiver(RXDevName)->GetRXGain());

	loss->SetArea(devHelper->GetReceiver(RXDevName)->GetPhotoDetectorArea());

	//this->m_channel[chName]->SetPropagationLossParametersFromRX(devHelper->GetReceiver(RXDevName));
}

ns3::Ptr<VlcChannel> VlcChannelHelper::GetChannel(std::string chName) {
	NS_LOG_FUNCTION(this);
	return m_channel[chName];
}

ns3::Ptr<VlcNetDevice> VlcChannelHelper::GetDevice(std::string chName,
		uint32_t idx) {
	NS_LOG_FUNCTION(this);
	ns3::Ptr < VlcNetDevice > ans = DynamicCast < VlcNetDevice
			> (m_channel[chName]->GetDevice(idx));
	return ans;
}

NetDeviceContainer VlcChannelHelper::Install(std::string chName, Ptr<Node> a,
		Ptr<Node> b) {
	NS_LOG_FUNCTION(this);
	NetDeviceContainer container;
	Ptr < VlcChannel > ch = this->m_channel[chName];

	Ptr < VlcTxNetDevice > devTX = DynamicCast < VlcTxNetDevice
			> (ch->GetDevice(0));
	Ptr < VlcRxNetDevice > devRX = DynamicCast < VlcRxNetDevice
			> (ch->GetDevice(1));

	devTX->SetAddress(Mac48Address::Allocate());
	a->AddDevice(devTX);
	Ptr < Queue > queueA = m_queueFactory.Create<Queue>();
	devTX->SetQueue(queueA);

	devRX->SetAddress(Mac48Address::Allocate());
	b->AddDevice(devRX);
	Ptr < Queue > queueB = m_queueFactory.Create<Queue>();
	devRX->SetQueue(queueB);

	devTX->AttachChannel(ch);
	devRX->AttachChannel(ch);

	container.Add(devTX);
	container.Add(devRX);

	return container;
}

NetDeviceContainer VlcChannelHelper::Install(Ptr<Node> a, Ptr<Node> b,
		Ptr<VlcTxNetDevice> tx, Ptr<VlcRxNetDevice> rx) {
	NetDeviceContainer container;

	tx->SetAddress(Mac48Address::Allocate());
	a->AddDevice(tx);
	Ptr < Queue > queueA = m_queueFactory.Create<Queue>();
	tx->SetQueue(queueA);

	rx->SetAddress(Mac48Address::Allocate());
	b->AddDevice(rx);
	Ptr < Queue > queueB = m_queueFactory.Create<Queue>();
	rx->SetQueue(queueB);

	bool useNormalChannel = true;
	Ptr < VlcChannel > channel = 0;

	if (useNormalChannel) {
		channel = m_channelFactory.Create<VlcChannel>();
	}

	tx->AttachChannel(channel);
	rx->AttachChannel(channel);
	container.Add(tx);
	container.Add(rx);

	return container;
}

NetDeviceContainer VlcChannelHelper::Install(Ptr<Node> a, Ptr<Node> b,
		Ptr<VlcDeviceHelper> devHelper, Ptr<VlcChannelHelper> chHelper,
		std::string txName, std::string rxName, std::string chName) {

	NetDeviceContainer container;
	devHelper->GetTransmitter(txName)->SetAddress(Mac48Address::Allocate());

	a->AddDevice(devHelper->GetTransmitter(txName));
	Ptr < Queue > queueA = m_queueFactory.Create<Queue>();
	devHelper->GetTransmitter(txName)->SetQueue(queueA);
	devHelper->GetReceiver(rxName)->SetAddress(Mac48Address::Allocate());

	b->AddDevice(devHelper->GetReceiver(rxName));
	Ptr < Queue > queueB = m_queueFactory.Create<Queue>();
	devHelper->GetReceiver(rxName)->SetQueue(queueB);

	bool useNormalChannel = true;

	Ptr < VlcChannel > channel = 0;
	if (useNormalChannel) {
		channel = chHelper->GetChannel(chName);
		m_subChannel = CreateObject<PointToPointChannel>();
	}

	devHelper->GetTransmitter(txName)->AttachChannel(channel);
	devHelper->GetTransmitter(txName)->Attach(m_subChannel);
	channel->Attach(devHelper->GetTransmitter(txName));

	devHelper->GetReceiver(rxName)->AttachChannel(channel);
	devHelper->GetReceiver(rxName)->Attach(m_subChannel);
	channel->Attach(devHelper->GetReceiver(rxName));

	container.Add(devHelper->GetTransmitter(txName));
	container.Add(devHelper->GetReceiver(rxName));

	return container;
}

VlcChannelHelper::~VlcChannelHelper() {
	m_channel.clear();
}

} /* namespace vlc */
