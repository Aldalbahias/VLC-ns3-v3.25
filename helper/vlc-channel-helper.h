#ifndef VLCCHANNELHELPER_H_
#define VLCCHANNELHELPER_H_

#include <map>
#include <iostream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/vlc-channel-model.h"
#include "ns3/vlc-device-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/queue.h"
#include "ns3/vlc-net-device.h"
#include "ns3/vlc-rx-net-device.h"
#include "ns3/vlc-tx-net-device.h"
#include "ns3/object-factory.h"

namespace ns3 {

class VlcChannelHelper: public Object {
public:
	VlcChannelHelper();

	void CreateChannel(std::string channelName);

	void SetChannelWavelength(std::string channelName, int lower, int upper);

	void SetPropagationLoss(std::string channelName,
			std::string propagationLossType);

	void SetPropagationDelay(std::string channelName, double value);

	void AttachTransmitter(std::string chName, std::string TXDevName,
			ns3::Ptr<VlcDeviceHelper> devHelper);

	void AttachReceiver(std::string chName, std::string RXDevName,
			ns3::Ptr<VlcDeviceHelper> devHelper);

	double GetChannelSNR(std::string chName);

	void SetChannelParameter(std::string chName, std::string paramName,
			double value);

	ns3::Ptr<VlcChannel> GetChannel(std::string chName);

	ns3::Ptr<VlcNetDevice> GetDevice(std::string chName, uint32_t idx);

	ns3::NetDeviceContainer Install(std::string chName, Ptr<Node> a,
			Ptr<Node> b);

	NetDeviceContainer Install(Ptr<Node> a, Ptr<Node> b, Ptr<VlcTxNetDevice> tx,
			Ptr<VlcRxNetDevice> rx);

	NetDeviceContainer Install(Ptr<Node> a, Ptr<Node> b,
			Ptr<VlcDeviceHelper> devHelper, Ptr<VlcChannelHelper> chHelper,
			std::string txName, std::string rxName, std::string chName);

	virtual ~VlcChannelHelper();

private:

	std::map<std::string, ns3::Ptr<VlcChannel> > m_channel;

	ObjectFactory m_queueFactory;
	ObjectFactory m_channelFactory;       //!< Channel Factory
	ObjectFactory m_remoteChannelFactory;
	Ptr<PointToPointChannel> m_subChannel;
};

} /* namespace vlc */

#endif /* VLCCHANNELHELPER_H_ */
