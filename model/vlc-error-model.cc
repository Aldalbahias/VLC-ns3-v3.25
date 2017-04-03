#include <cmath>
#include "vlc-error-model.h"
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/enum.h"
#include "ns3/integer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/pointer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("VlcErrorModel");// define a log component with the name "VlcErrorModel"
NS_OBJECT_ENSURE_REGISTERED (VlcErrorModel);// register VlcErrorModel class with the TypeId system

TypeId VlcErrorModel::GetTypeId(void)// returns meta-information about VlcErrorModel class 
		{ 	// including parent class, group name, constructor, and attributes
	static TypeId tid =
			TypeId("ns3::VlcErrorModel").SetParent<RateErrorModel>().SetGroupName(
					"Network").AddConstructor<VlcErrorModel>().AddAttribute(
					"ModulationOrder", "The modulation order (M)",
					IntegerValue(0),
					MakeIntegerAccessor(&VlcErrorModel::mod_order),
					MakeIntegerChecker<int>()).AddAttribute("Alpha",
					"The duty cycle of the VPPM signal", DoubleValue(0.0),
					MakeDoubleAccessor(&VlcErrorModel::alpha),
					MakeDoubleChecker<double>()).AddAttribute("Beta",
					"A factor relating noise bandwidth to signal bandwidth",
					DoubleValue(0.0), MakeDoubleAccessor(&VlcErrorModel::beta),
					MakeDoubleChecker<double>()).AddAttribute("SNR",
					"Signal-to-Noise Ratio", DoubleValue(0.0),
					MakeDoubleAccessor(&VlcErrorModel::SNR),
					MakeDoubleChecker<double>()).AddAttribute("RanVar1",
					"The decision variable attached to this error model.",
					StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
					MakePointerAccessor(&VlcErrorModel::m_ranvar),
					MakePointerChecker<RandomVariableStream>()).AddAttribute(
					"ModulationScheme",
					"The modulation scheme used in this model", EnumValue(PAM4),
					MakeEnumAccessor(&VlcErrorModel::mod_scheme),
					MakeEnumChecker(PAM4, "PAM4", OOK, "OOK", VPPM, "VPPM"));
	return tid;
}

// constructor
VlcErrorModel::VlcErrorModel() {

	NS_LOG_FUNCTION(this);

	alpha = 0;
	beta = 1;
	SNR = 0;
	mod_order = 0;
	mod_scheme = OOK;

}

// destructor
VlcErrorModel::~VlcErrorModel() {
	NS_LOG_FUNCTION(this);
}

VlcErrorModel::ModScheme VlcErrorModel::GetScheme(void) const// returns the modulation scheme used
		{
	NS_LOG_FUNCTION(this);
	return mod_scheme;
}

void VlcErrorModel::SetScheme(ModScheme scheme)	// sets the value of the modulation scheme used
		{
	NS_LOG_FUNCTION(this << scheme);
	mod_scheme = scheme;
	if (scheme == OOK){
		mod_order = 2;
	}
	else if (scheme == PAM4){
		mod_order = 4;
	}
	else if (scheme == PAM8){
	    mod_order = 8;
	}
	else if (scheme == PAM16){
		mod_order = 16;
	}

}


// SetRandomVariable method assigns a random variable stream to be used by this model
void VlcErrorModel::SetRandomVariable(Ptr<RandomVariableStream> ranvar) {
	NS_LOG_FUNCTION(this << ranvar);
	m_ranvar = ranvar;
}

//AssignStreams method assigns a fixed stream number to the random variables used by this model
int64_t VlcErrorModel::AssignStreams(int64_t stream) {
	NS_LOG_FUNCTION(this << stream);
	m_ranvar->SetStream(stream);
	return 1;
}

// GetModulationOrder method returns the modulation order (M)
int VlcErrorModel::GetModulationOrder(void) const {
	NS_LOG_FUNCTION(this);
	return mod_order;
}

// SetModulationOrder method sets the modulation order value
void VlcErrorModel::SetModulationOrder(int m_order) {
	NS_LOG_FUNCTION(this << m_order);
	mod_order = m_order;
}

// GetAlpha method returns alpha value
double VlcErrorModel::GetAlpha(void) const {
	NS_LOG_FUNCTION(this);
	return alpha;
}

// SetAlpha method sets alpha value
void VlcErrorModel::SetAlpha(double a) {
	NS_LOG_FUNCTION(this << a);
	alpha = a;
}

// GetBeta method returns beta value
double VlcErrorModel::GetBeta(void) const {
	NS_LOG_FUNCTION(this);
	return beta;
}

// SetBeta method sets beta value
void VlcErrorModel::SetBeta(double b) {
	NS_LOG_FUNCTION(this << b);
	beta = b;
}

// GetSNR method returns the signal-to-noise ratio (SNR) value
double VlcErrorModel::GetSNR(void) const {
	NS_LOG_FUNCTION(this);
	return SNR;
}

// SetSNR method sets the SNR value
void VlcErrorModel::SetSNR(double snr) {
	NS_LOG_FUNCTION(this << snr);
	SNR = snr;
}

// CalculateErrorRate calculates SER or BER value according to modulation scheme
double VlcErrorModel::CalculateErrorRate(void) {
	NS_LOG_FUNCTION(this);
	if (mod_scheme == VPPM)
		return CalculateBER();
	else if (mod_scheme == PAM4 || mod_scheme == OOK || mod_scheme == PAM8 || mod_scheme == PAM16 ) // OOK IS A SUBSET OF PAM
		return CalculateSER();
	else if (mod_scheme == PSK4 || mod_scheme == PSK16 || mod_scheme == QAM4 || mod_scheme == QAM16)
			return ComputeBER();

	return 0;
}

// CalculateSER method calculates the SER value of OOK and PAM models
// using modulation order(M) and SNR
double VlcErrorModel::CalculateSER(void) {
	NS_LOG_FUNCTION(this);
	// Calculate Q(sqrt(SNR) / (M-1))
	// Q-function is the tail probability of the standard normal distribution
	double Q = 0.5 * erfc((std::sqrt(SNR) / (mod_order - 1)) / std::sqrt(2));
	double SER = (2 * (mod_order - 1) / mod_order) * Q;
	SetRate(SER);
	if (mod_scheme == OOK)
		SetUnit (ERROR_UNIT_BIT);
	return SER;
}

// CalculateBER method calculates the BER of VPPM model using alpha, beta and SNR
double VlcErrorModel::CalculateBER(void) {
	NS_LOG_FUNCTION(this);
	double x; // the vaue to calculate Q(x)
	if(alpha==0){
		throw std::logic_error("alpha should be set for VPPM modulation scheme and cannot be zero");
	}
	if (alpha <= 0.5)
		x = std::sqrt(SNR / (2 * beta * alpha));
	else
		x = std::sqrt(SNR * (1 - alpha) / (2 * beta * std::pow(alpha, 2)));
	// Calculate BER = Q(x)
	// Q-function is the tail probability of the standard normal distribution
	double Q = 0.5 * erfc(x / std::sqrt(2));
	SetRate(Q);	// call SetRate method in parent class RateErrorModel to set m_rate to BER value
	SetUnit (ERROR_UNIT_BIT);
	return Q;
}

// DoCorrupt method determines if the packet is corrupted according to the error model used
bool VlcErrorModel::IsCorrupt(Ptr<Packet> p) {
	NS_LOG_FUNCTION(this << p);

	if (mod_scheme == PAM4 || mod_scheme == OOK || mod_scheme == PAM8 || mod_scheme == PAM16) {
		// check if the model is enabled
		if (!IsEnabled()) {
			return false;
		}

		// computes pkt error rate from symbol error rate
		double symbol_size = log2(mod_order);	// symbol size in bits
		double symbols_per_pkt = static_cast<double>(8 * p->GetSize())/ symbol_size; // no. of symbols per packet
		// Compute pkt error rate by finding the complement of the probablility 
		// that a packets is not corrupted
		// = (1 - the probability that all symbols in pkt are not corrupted)
		double per = 1 - std::pow(1.0 - GetRate(), symbols_per_pkt);
		// the pkt is corrupted according to PER
		// using random variable m_ranvar
		return (m_ranvar->GetValue() < per);
	}

	// for VPPM scheme, call the method defined in parent class RateErrorModel
	else {
		RateErrorModel::SetRandomVariable (m_ranvar);
		return RateErrorModel::IsCorrupt(p);
	}
}

double VlcErrorModel::GenerateQfunction(double a) {
	//double d = a*100 - 155;
	double d = a;
	return 0.5 * erfc(d / sqrt(2));
}

double VlcErrorModel::ComputeBER() {

	//specific to PSK and QAM
	double snr = SNR;

	double errorRate = 0;
	if(this->mod_scheme==QAM4){
		errorRate = sqrt(snr);
		errorRate = this->GenerateQfunction(errorRate);
	}
	else if(this->mod_scheme==QAM16){
		errorRate = sqrt(snr/5.0);
		errorRate = this->GenerateQfunction(errorRate);
		errorRate *= 0.75;
	}
	else if(this->mod_scheme==PSK4){
		errorRate = sqrt(2.0*snr) * sin(M_PI/4.0);
		errorRate = this->GenerateQfunction(errorRate);
	}
	else if(this->mod_scheme==PSK16){
		errorRate = sqrt(2.0*snr) * sin(M_PI/16.0);
		errorRate = this->GenerateQfunction(errorRate);
		errorRate *= 0.5;
	}

	SetUnit (ERROR_UNIT_BIT);

	return errorRate;

}

bool VlcErrorModel::CorruptPacket(Ptr<Packet> p, double ber) {
	RateErrorModel::SetRate(ber);
	bool val = RateErrorModel::IsCorrupt(p);
	return val;
}

double VlcErrorModel::GetAveragePower(double pmax,double pmin,double alpha){
	if(mod_scheme== VPPM){
		return alpha*pmax;;
	}
	else {
		return (pmax+pmin)/2;
	}
	return 0;
}

// DoReset method does nothing
void VlcErrorModel::DoReset(void) {
	NS_LOG_FUNCTION(this);
	/* re-initialize any state; no-op for now */
}

} // namespace ns3
