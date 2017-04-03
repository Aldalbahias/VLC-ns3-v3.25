#ifndef VLC_CHANNEL_HELPER_H_
#define VLC_CHANNEL_HELPER_H_

#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/pointer.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/vlc-propagation-loss-model.h"
#include "ns3/vlc-snr.h"
#include "ns3/vlc-error-model.h"
#include "ns3/log.h"
#include "ns3/vlc-net-device.h"
#include "ns3/vlc-tx-net-device.h"
#include "ns3/vlc-rx-net-device.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include <cmath>
#include <algorithm>

namespace ns3 {

class VlcChannel: public Channel {
public:
	VlcChannel();

	virtual ~VlcChannel();

	static TypeId GetTypeId(void);

	virtual uint32_t GetNDevices(void) const; //gets the number of devices attached to the channel.

	virtual Ptr<NetDevice> GetDevice(uint32_t i) const; //gets the device specidied by the index number.

	void SetPropagationLossModel(ns3::Ptr<ns3::PropagationLossModel> loss); //sets the propagation loss model for the channel

	ns3::Ptr<ns3::PropagationLossModel> GetPropagationLossModel();//returns a pointer to the propagation loss model

	void SetPropagationDelayModel(ns3::Ptr<ns3::PropagationDelayModel> delay); //sets the propagation delay model for the channel

	ns3::Ptr<ns3::PropagationDelayModel> GetPropagationDelayModel(); //returns the propagation delay model of the channel

	void SetPropagationDelay(double delay); 

	double GetDistance(); //returns the distance after computing the distance using the mobility models of TX and RX

	void SetDistance();	//sets the distance after computing the distance using the mobility models of TX and RX

	void DoCalcPropagationLoss();//calculates the optical power signal for the RX using the optical power signal of the TX and loss model

	double DoCalcPropagationLossForSignal(int timeInstant);	//computes the optical power signal of RX at a particular instant

	void SetAveragePower(double power);	//computes the average power of the signals

	double GetAveragePower();		//returns the average power of the signals

	void SetWavelength(int lower, int upper);// sets upper and lower bound wavelength [nm]

	void SetTemperature(double t);	// sets the black body temperature of LED

	double GetTemperature(); //returns the temperature value as double.

	void SetReceivedPower(double p);// sets the average received optical signal power

	void CalculateNoiseVar();	//calculates the noise variance

	void CalculateSNR();		// calculates the SNR value

	double GetSNR() const;		// returns the signal-to-noise ratio (SNR)

	void SetElectricNoiseBandWidth(double b);	// sets the noise bandwidth

	double GetNoiseBandwidth();	//return the noise bandwidth

	bool TransmitStart(Ptr<Packet> p, Ptr<VlcNetDevice> src, Time txTime); //starts the transmission of packets.

	double GetDistance(ns3::Ptr<ns3::MobilityModel> aTX,
	ns3::Ptr<ns3::MobilityModel> bRX) const; //returns the distance between sender and receiver devices.

	void TransmitDataPacket(Ptr<Packet> p); //starts the transmission of data packets.

	void EnqueueDataPacketAfterCorruption(); 

	void Attach(Ptr<VlcNetDevice> device); 

private:
	ns3::Ptr<ns3::PropagationLossModel> m_loss;

	double m_distanceBWTXandRX;

	ns3::Ptr<ns3::PropagationDelayModel> m_delay;

	double m_AvgPower;

	ns3::Ptr<ns3::VlcSnr> m_SNR;

	uint32_t m_nDevices; //number of devices.

	TracedCallback<Ptr<const Packet>, Ptr<NetDevice>, Ptr<NetDevice>, Time, Time> m_txrxVlcChannel;

	class WirelessLink { //wireless link class for storing a sender and receiver vlc device.
	public:
		WirelessLink() :
				m_src(0), m_dst(0) {
		}

		Ptr<VlcNetDevice> m_src;   //!< First NetDevice
		Ptr<VlcNetDevice> m_dst;   //!< Second NetDevice
	};

	WirelessLink m_link[2];
};

} /* namespace vlc */

#endif /* SRC_VLCNEW_VLCCHANNEL_H_ */
