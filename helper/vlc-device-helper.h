#ifndef VLCDEVICEHELPER_H_
#define VLCDEVICEHELPER_H_

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include "ns3/vlc-rx-net-device.h"
#include "ns3/vlc-tx-net-device.h"
#include "ns3/core-module.h"
#include "ns3/vlc-error-model.h"

namespace ns3 {

class VlcDeviceHelper: public Object {
public:
	VlcDeviceHelper();

	~VlcDeviceHelper();

	void CreateTransmitter(std::string TXName);

	void CreateReceiver(std::string RXName);

	ns3::Ptr<VlcTxNetDevice> GetTransmitter(std::string devName);

	ns3::Ptr<VlcRxNetDevice> GetReceiver(std::string devName);

	std::vector<double> GenerateSignal(int size, double dutyRatio, double bias,
			double VMax, double VMin);

	void SetTXSignal(std::string devName, int size, double dutyRatio,
			double bias, double VMax, double VMin);

	void SetTrasmitterParameter(std::string devName, std::string paramName,
			double value);

	void SetTrasmitterBoost(std::string devName);

	void SetTrasmitterPosition(std::string devName, double x, double y,
			double z);

	void SetReceiverPosition(std::string devName, double x, double y, double z);

	void SetReceiverParameter(std::string devName, std::string paramName,
			double value);

	double GetReceiverParameter(std::string devName, std::string paramName);

private:

	std::map<std::string, ns3::Ptr<VlcTxNetDevice> > m_TXDevices;

	std::map<std::string, ns3::Ptr<VlcRxNetDevice> > m_RXDevices;

};

} /* namespace vlc */

#endif /* VLCDEVICEHELPER_H_ */
