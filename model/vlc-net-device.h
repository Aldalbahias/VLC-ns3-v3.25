#ifndef VLCNETDEVICE_H_
#define VLCNETDEVICE_H_

#include "ns3/point-to-point-net-device.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/vlc-mobility-model.h"
#include "ns3/mobility-model.h"
#include "ns3/pointer.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/object.h"
#include "ns3/type-id.h"
#include <cstring>
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "ns3/error-model.h"
#include "ns3/queue.h"
#include "ns3/log.h"
#include "ns3/object.h"

// using namespace ns3;

namespace ns3 {

class VlcNetDevice: public ns3::PointToPointNetDevice {

public:
	VlcNetDevice();

	static ns3::TypeId GetTypeId(void);

	virtual ~VlcNetDevice();

	//returns the azmuth, i.e. facing of the device
	double GetAzmuth();

	//sets the azmuth,i.e. facing of the device
	void SetAzmuth(double az);

	//returns the x,y,z co-ordinates of the device
	ns3::Vector GetPosition();

	//sets the position of the device to a particular x,y,z value
	void SetPosition(ns3::Vector position);

	//returns the elevation of the device
	double GetElevation();

	//sets the elevation of the device
	void SetElevation(double elevation);

	//returns a pointer to the mobility model of the device
	ns3::Ptr<VlcMobilityModel> GetMobilityModel();

	//sets the mobility model of the device
	void SetMobilityModel(ns3::Ptr<VlcMobilityModel> model);

	virtual void Receive(Ptr<Packet> p);

	virtual bool Send(Ptr<Packet> packet, const Address &dest,
			uint16_t protocolNumber);

private:
	ns3::Ptr<VlcMobilityModel> m_mobilityModel;
};

} /* namespace vlc */

#endif /* VLCNETDEVICE_H_ */
