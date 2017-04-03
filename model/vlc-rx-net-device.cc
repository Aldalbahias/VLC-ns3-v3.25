#include "ns3/vlc-rx-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("vlcNetDeviceRX");

NS_OBJECT_ENSURE_REGISTERED (VlcRxNetDevice);

ns3::TypeId VlcRxNetDevice::GetTypeId(void)	// returns meta-information about VlcErrorModel class
		{ 	// including parent class, group name, constructor, and attributes

	static ns3::TypeId tid = ns3::TypeId("VlcRxNetDevice").SetParent<
			VlcNetDevice>().AddConstructor<VlcRxNetDevice>().AddAttribute(
			"FilterGain", "filter gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_filterGain),
			MakeDoubleChecker<double>()).AddAttribute("PhotoDetectorArea",
			"photo detector area for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_photodetectorArea),
			MakeDoubleChecker<double>()).AddAttribute("FOVAngle",
			"field of view angle for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_FOVangle),
			MakeDoubleChecker<double>()).AddAttribute("RefractiveIndex",
			"refractive index of the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_refIndex),
			MakeDoubleChecker<double>()).AddAttribute("AngleOfIncidence",
			"angle of incidence of the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_angleOfIncidence),
			MakeDoubleChecker<double>()).AddAttribute("ConcentrationGain",
			"concentration gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_concentrationGain),
			MakeDoubleChecker<double>()).AddAttribute("RXGain",
			"RX gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_RXGain),
			MakeDoubleChecker<double>()).AddAttribute("Bias",
			"Bias for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_bias),
			MakeDoubleChecker<double>())

			;

	return tid;

}

VlcRxNetDevice::VlcRxNetDevice() {

	//NS_LOG_FUNCTION(this);

	m_filterGain = 0;
	m_photodetectorArea = 0;
	m_FOVangle = 0;
	m_refIndex = 0;
	m_angleOfIncidence = 0;
	m_concentrationGain = 0;
	m_RXGain = 0;
	m_bias = 0;
	m_error = ns3::CreateObject<ns3::VlcErrorModel>();

}

void ns3::VlcRxNetDevice::AttachChannel(Ptr<VlcChannel> channel) {
	m_channel = channel;
}

//use this function after setting up FOV and refractive index
void VlcRxNetDevice::SetConcentrationGain() {
	NS_LOG_FUNCTION(this);
	this->m_concentrationGain = std::pow(this->m_refIndex, 2)
			/ std::pow((float) std::sin((float) m_FOVangle), 2);
}

void VlcRxNetDevice::AddRXOpticalPowerSignal(double power) {
	NS_LOG_FUNCTION(this<<power);
	this->m_RXOpticalPower.push_back(power);
}
std::vector<double>& VlcRxNetDevice::GetRXOpticalPowerSignal() {
	NS_LOG_FUNCTION(this);
	return this->m_RXOpticalPower;
}
double VlcRxNetDevice::GetOpticalPowerSignalAtInstant(int time) {
	NS_LOG_FUNCTION(this<<time);
	return this->m_RXOpticalPower.at(time);
}
void VlcRxNetDevice::SetRXOpticalPowerSignal(std::vector<double> &powerSignal) {
	NS_LOG_FUNCTION(this<<powerSignal.size());
	this->m_RXOpticalPower = powerSignal;
}

void VlcRxNetDevice::SetCapacity(int size) {
	NS_LOG_FUNCTION(this<<size);
	this->m_signal.reserve(size);
	this->m_RXOpticalPower.reserve(size);
}

double VlcRxNetDevice::GetFilterGain() {
	NS_LOG_FUNCTION(this);
	return this->m_filterGain;
}
void VlcRxNetDevice::SetFilterGain(double fgain) {
	NS_LOG_FUNCTION(this<<fgain);
	this->m_filterGain = fgain;
}

double VlcRxNetDevice::GetPhotoDetectorArea() {
	NS_LOG_FUNCTION(this);
	return this->m_photodetectorArea;
}

void VlcRxNetDevice::SetPhotoDectectorArea(double pArea) {
	NS_LOG_FUNCTION(this<<pArea);
	this->m_photodetectorArea = pArea;
}

double VlcRxNetDevice::GetFOVAngle() {
	NS_LOG_FUNCTION(this);
	return this->m_FOVangle;
}

void VlcRxNetDevice::SetFOVAngle(double angle) {
	NS_LOG_FUNCTION(this<<angle);
	this->m_FOVangle = angle * M_PI / 180;
}

double VlcRxNetDevice::GetRefractiveIndex() {
	NS_LOG_FUNCTION(this);
	return this->m_refIndex;
}
void VlcRxNetDevice::SetRefractiveIndex(double angle) {
	NS_LOG_FUNCTION(this << angle);
	this->m_refIndex = angle;
}

double VlcRxNetDevice::GetConcentrationGain() {
	NS_LOG_FUNCTION(this);
	return this->m_concentrationGain;
}

double VlcRxNetDevice::GetRXGain() {
	NS_LOG_FUNCTION(this);
	return this->m_RXGain;
}
void VlcRxNetDevice::SetRXGain() {
	NS_LOG_FUNCTION(this);
	this->m_RXGain = std::cos(this->m_angleOfIncidence);
}

void VlcRxNetDevice::SetIncidenceAngle(double angle) {
	NS_LOG_FUNCTION(this << angle);
	this->m_angleOfIncidence = angle * M_PI / 180;
}

ns3::VlcErrorModel::ModScheme VlcRxNetDevice::GeModulationtScheme(void) const {	// returns the modulation scheme used
	NS_LOG_FUNCTION(this);
	return this->m_error->GetScheme();
}

void VlcRxNetDevice::SetScheme(VlcErrorModel::ModScheme scheme) { // sets the value of the modulation scheme used
	NS_LOG_FUNCTION(this<<scheme);
	this->m_error->SetScheme(scheme);

}

void VlcRxNetDevice::SetRandomVariableForErrorModel(
	ns3::Ptr<ns3::RandomVariableStream> ranVar) {// assigns a random variable stream to be used by this model
	NS_LOG_FUNCTION(this<<ranVar);
	this->m_error->SetRandomVariable(ranVar);
}

int64_t VlcRxNetDevice::AssignStreamsForErrorModel(int64_t stream) {// assigns a fixed stream number to the random variables used by this model
	NS_LOG_FUNCTION(this);
	return this->m_error->AssignStreams(stream);
}

double VlcRxNetDevice::GetSNRFromErrorModel(void) const {// returns the signal-to-noise ratio (SNR)
	NS_LOG_FUNCTION(this);
	return this->m_error->GetSNR();
}

void VlcRxNetDevice::SetSNRForErrorModel(double snr) {	// sets the SNR value
	NS_LOG_FUNCTION(this<<snr);
	this->m_error->SetSNR(snr);
}

double VlcRxNetDevice::CalculateErrorRateForErrorModel() {// calculates the error rate value according to modulation scheme
	NS_LOG_FUNCTION(this);
	return this->m_error->CalculateErrorRate();
}

bool VlcRxNetDevice::IsCorrupt(ns3::Ptr<ns3::Packet> pkt) {	// determines if the packet is corrupted according to the error model
	NS_LOG_FUNCTION(this<<pkt);
	return this->m_error->IsCorrupt(pkt);
}

// methods for PAM
int VlcRxNetDevice::GetModulationOrder(void) const {// returns the modulation order (M)
	NS_LOG_FUNCTION(this);
	return this->m_error->GetModulationOrder();
}

void VlcRxNetDevice::SetModulationOrder(int m_order) {// sets the modulation order value
	NS_LOG_FUNCTION(this<<m_order);
	return this->m_error->SetModulationOrder(m_order);
}

// methods for VPPM
double VlcRxNetDevice::GetAlpha(void) const {	// returns alpha value
	NS_LOG_FUNCTION(this);
	return this->m_error->GetAlpha();
}
void VlcRxNetDevice::SetAlpha(double a) {		// sets alpha value
	NS_LOG_FUNCTION(this);
	if( m_error->GetScheme()!=VlcErrorModel::VPPM ){
		throw std::logic_error("Alpha should not be set for these modulation schemes \n");
	}
	this->m_error->SetAlpha(a);
}

double VlcRxNetDevice::GetBeta(void) const {		// returns beta value
	NS_LOG_FUNCTION(this);
	return this->m_error->GetBeta();
}
void VlcRxNetDevice::SetBeta(double b) {		// sets beta value
	NS_LOG_FUNCTION(this<<b);
	return this->m_error->SetBeta(b);
}

double VlcRxNetDevice::GetIncidenceAngle() {
	NS_LOG_FUNCTION(this);
	return this->m_angleOfIncidence;
}

ns3::Ptr<ns3::VlcErrorModel> VlcRxNetDevice::GetErrorModel() {
	NS_LOG_FUNCTION(this);
	return this->m_error;
}

void VlcRxNetDevice::EnqueueDataPacketAfterCorruption(Ptr<Packet> p,
		bool corruptFlag) {
	NS_LOG_FUNCTION(this<<p<<corruptFlag);
	this->dataPool.push_back(p);
	this->packetCorruptionState.push_back(corruptFlag);
}

int VlcRxNetDevice::ComputeGoodPut() {
	NS_LOG_FUNCTION(this);
	int goodPacketSize = 0;
	for (uint32_t i = 0; i < this->dataPool.size(); i++) {
		if (!this->packetCorruptionState.at(i)) {
			goodPacketSize += this->dataPool.at(i)->GetSize();
		}
	}

	return goodPacketSize;

}

VlcRxNetDevice::~VlcRxNetDevice() {
	// TODO Auto-generated destructor stub
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////

} /* namespace vlc */
