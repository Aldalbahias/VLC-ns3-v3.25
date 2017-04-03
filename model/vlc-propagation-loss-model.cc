#include "ns3/vlc-propagation-loss-model.h"
#include <cmath>

namespace ns3 {

VlcPropagationLossModel::VlcPropagationLossModel() {
	m_TxPowerMAX = 0;
	m_TxPower = 0;
	m_FilterGain = 0;
	m_ConcentratorGain = 0;
	m_RxPower = 0;

	m_RXGain = 0;
	m_TXGain = 0;

}

void VlcPropagationLossModel::SetTxPower(double watt) {
	this->m_TxPower = watt;
}
void VlcPropagationLossModel::SetTxPowerMAX(double MAX) {
	this->m_TxPowerMAX = MAX;
}

double VlcPropagationLossModel::GetTxPower() {
	return this->m_TxPower;
}

double VlcPropagationLossModel::GetTxPowerMAX() {
	return this->m_TxPowerMAX;
}

void VlcPropagationLossModel::SetFilterGain(double gain) {
	this->m_FilterGain = gain;
}

double VlcPropagationLossModel::GetFilterGain() {
	return this->m_FilterGain;
}

void VlcPropagationLossModel::SetConcentratorGain(double cGain) {
	this->m_ConcentratorGain = cGain;
}

double VlcPropagationLossModel::GetConcentratorGain() {
	return this->m_ConcentratorGain;
}

double VlcPropagationLossModel::DoCalcRxPower(double txPowerDbm,
		ns3::Ptr<ns3::MobilityModel> aTX,
		ns3::Ptr<ns3::MobilityModel> bRX) const {
	double distance = this->GetDistance(aTX, bRX);
	double pRX = txPowerDbm * this->m_RXGain * this->m_TXGain
			* this->m_FilterGain * this->m_ConcentratorGain * this->m_Area;
	pRX /= std::pow(distance, 2);
	return pRX;
}

double VlcPropagationLossModel::GetRxPower() {
	return this->m_RxPower;
}

void VlcPropagationLossModel::SetRXPower(double wattTX,
		ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX) {
	this->m_RxPower = this->DoCalcRxPower(wattTX, aTX, bRX);
}

int64_t VlcPropagationLossModel::DoAssignStreams(int64_t stream) {
	return stream;
}

double VlcPropagationLossModel::GetDistance(ns3::Ptr<ns3::MobilityModel> aTX,
		ns3::Ptr<ns3::MobilityModel> bRX) const {
	double dist = 0;
	Vector tx = aTX->GetPosition();
	Vector rx = bRX->GetPosition();
	dist = std::pow((tx.x - rx.x), 2) + std::pow((tx.y - rx.y), 2)
			+ std::pow((tx.z - rx.z), 2);
	dist = std::sqrt(dist);
	return dist;
}

void VlcPropagationLossModel::SetTXGain(double txgain) {
	this->m_TXGain = txgain;
}

double VlcPropagationLossModel::GetTXGain() {
	return this->m_TXGain;
}

void VlcPropagationLossModel::SetRXGain(double rxgain) {
	this->m_RXGain = rxgain;
}

double VlcPropagationLossModel::GetRXGain() {
	return this->m_RXGain;
}

void VlcPropagationLossModel::SetArea(double a) {
	this->m_Area = a;
}

double VlcPropagationLossModel::GetArea() {
	return this->m_Area;
}

VlcPropagationLossModel::~VlcPropagationLossModel() {

}

} /* namespace vlc */

