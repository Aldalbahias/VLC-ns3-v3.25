#include "vlc-device-helper.h"
#include <stdexcept>

namespace ns3 {

VlcDeviceHelper::VlcDeviceHelper() {

}

void VlcDeviceHelper::CreateTransmitter(std::string TXName) {
	m_TXDevices[TXName] = CreateObject<VlcTxNetDevice>();
}

void VlcDeviceHelper::CreateReceiver(std::string RXName) {
	m_RXDevices[RXName] = CreateObject<VlcRxNetDevice>();
}

void VlcDeviceHelper::SetTrasmitterParameter(std::string devName,
		std::string paramName, double value) {

	if (paramName == "SemiAngle") {
		m_TXDevices[devName]->SetSemiangle(value);
	} else if (paramName == "AngleOfRadiance") {
		m_TXDevices[devName]->SetAngleOfRadiance(value);
	} else if (paramName == "LambertianOrder") {
		m_TXDevices[devName]->SetLambertainOrder();
	} else if (paramName == "Gain") {
		m_TXDevices[devName]->SetTXGain();
	} else if (paramName == "Bias") {
		m_TXDevices[devName]->SetBias(value);
		m_TXDevices[devName]->BoostSignal();
	} else if (paramName == "Azimuth") {
		m_TXDevices[devName]->SetAzmuth(value);
	} else if (paramName == "Elevation") {
		m_TXDevices[devName]->SetElevation(value);
	} else if (paramName == "DataRateInMBPS") {
		std::ostringstream strs;
		strs << value;
		std::string str = strs.str();
		str = str + "Mbps";
		DataRate drate(str);
		m_TXDevices[devName]->SetDataRate(drate);
	} else if (paramName == "FrameSize") {
// 		std::logic_error("Frame Size not present");
		std::cout << "Frame Size not present" << std::endl;

	}

}

void VlcDeviceHelper::SetTrasmitterPosition(std::string devName, double x,
		double y, double z) {
	m_TXDevices[devName]->SetPosition(Vector(x, y, z));
}

void VlcDeviceHelper::SetReceiverPosition(std::string devName, double x,
		double y, double z) {
	m_RXDevices[devName]->SetPosition(Vector(x, y, z));
}

void VlcDeviceHelper::SetTrasmitterBoost(std::string devName) {
	m_TXDevices[devName]->BoostSignal();
}

void VlcDeviceHelper::SetReceiverParameter(std::string devName,
		std::string paramName, double value) {
	if (paramName == "FilterGain") {
		m_RXDevices[devName]->SetFilterGain(value);
	} else if (paramName == "PhotoDetectorArea") {
		m_RXDevices[devName]->SetPhotoDectectorArea(value);
	} else if (paramName == "FOVAngle") {
		m_RXDevices[devName]->SetFOVAngle(value);
	} else if (paramName == "RefractiveIndex") {
		m_RXDevices[devName]->SetRefractiveIndex(value);
	} else if (paramName == "IncidenceAngle") {
		m_RXDevices[devName]->SetIncidenceAngle(value);
	} else if (paramName == "ConcentrationGain") {
		m_RXDevices[devName]->SetConcentrationGain();
	} else if (paramName == "RXGain") {
		m_RXDevices[devName]->SetRXGain();
	} else if (paramName == "SetModulationScheme") {
		if (value == 0) {
			m_RXDevices[devName]->SetScheme(VlcErrorModel::PAM4);
		}
		else if (value == 1) {
			m_RXDevices[devName]->SetScheme(VlcErrorModel::OOK);
		}
		else if (value == 2) {
			m_RXDevices[devName]->SetScheme(VlcErrorModel::VPPM);
		}
		else if (value == 3) {
			m_RXDevices[devName]->SetScheme(VlcErrorModel::PSK4);
		}
		else if (value == 4) {
			m_RXDevices[devName]->SetScheme(VlcErrorModel::PSK16);
		}
		else if (value == 5) {
			m_RXDevices[devName]->SetScheme(VlcErrorModel::QAM4);
		}
		else if (value == 5) {
			m_RXDevices[devName]->SetScheme(VlcErrorModel::QAM16);
		}
		else{
			throw std::logic_error("Modulation scheme is not available\n");
		}

	} else if (paramName == "DutyCycle") {
		m_RXDevices[devName]->SetAlpha(value);
	} else if (paramName == "Beta") {
		m_RXDevices[devName]->SetBeta(value);
	}

}

double VlcDeviceHelper::GetReceiverParameter(std::string devName,
		std::string paramName) {
	if (paramName == "BER") {
		return this->m_RXDevices[devName]->CalculateErrorRateForErrorModel();
	} else if (paramName == "SER") {
		return this->m_RXDevices[devName]->CalculateErrorRateForErrorModel();
	}
	return 0;
}

std::vector<double> VlcDeviceHelper::GenerateSignal(int size, double dutyRatio,
		double bias, double VMax, double VMin) {

	std::vector<double> result;
	result.reserve(size);

	for (int i = 0; i < size; i++) {

		if (i < size * dutyRatio) {
			result.push_back(VMax + bias);
		} else {
			result.push_back(VMin + bias);
		}
	}

	return result;

}

void VlcDeviceHelper::SetTXSignal(std::string devName, int size,
		double dutyRatio, double bias, double VMax, double VMin) {

	std::vector<double> r = GenerateSignal(size, dutyRatio, bias, VMax, VMin);
	if(VMax>m_TXDevices[devName]->GetTXPowerMax()){
		throw std::logic_error("Power higher than device TX Power");
	}
	if(VMin<0){
		throw std::logic_error("Negative minimum power is not allowed");
	}
	m_TXDevices[devName]->SetSignal(r);

}

ns3::Ptr<VlcTxNetDevice> VlcDeviceHelper::GetTransmitter(std::string devName) {

	return this->m_TXDevices[devName];
}

ns3::Ptr<VlcRxNetDevice> VlcDeviceHelper::GetReceiver(std::string devName) {

	return this->m_RXDevices[devName];
}

VlcDeviceHelper::~VlcDeviceHelper() {
	this->m_RXDevices.clear();
	this->m_TXDevices.clear();
}

} /* namespace vlc */
