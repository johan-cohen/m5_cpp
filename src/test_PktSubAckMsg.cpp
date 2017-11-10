
#include "test_Common.hpp"
#include "PktSubAck.hpp"

template <typename T>
int test(m5::ReasonCode *codes, std::size_t size)
{
	T *pkt;
	const char msg[] = "Hello, World!";
	m5::AppBuf buf(256);

	pkt = new T();
	pkt->packetId(0xABCD);

	for (std::size_t i = 0; i < size; i++) {
		pkt->append(codes[i]);
	}

	pkt->properties.userProperty(msg, msg);
	pkt->properties.reasonString(msg);
	pkt->properties.userProperty(msg, msg);
	pkt->writeTo(buf);

	T pktRead(buf);

	if (pkt->packetId() != pktRead.packetId()) {
		throw std::logic_error("read/write: Packet Id");
	}
	if (pkt->reasonCodes() != pktRead.reasonCodes()) {
		throw std::logic_error("read/write: Reason Codes");
	}

	delete pkt;

	return 0;
}

int main(void)
{
	m5::ReasonCode codes_suback[] = {
		m5::ReasonCode::GRANTED_QOS0,
		m5::ReasonCode::GRANTED_QOS1,
		m5::ReasonCode::UNSPECIFIED_ERROR,
		m5::ReasonCode::IMPLEMENTATION_SPECIFIC_ERROR,
		m5::ReasonCode::NOT_AUTHORIZED,
		m5::ReasonCode::TOPIC_FILTER_INVALID,
		m5::ReasonCode::PACKET_IDENTIFIER_IN_USE,
		m5::ReasonCode::QUOTA_EXCEEDED,
		m5::ReasonCode::SHARED_SUBSCRIPTION_NOT_SUPPORTED,
		m5::ReasonCode::SUBSCRIPTION_IDENTIFIERS_NOT_SUPPORTED };
	int rc;

	rc = test<m5::PktSubAck>(codes_suback, sizeof(codes_suback) / sizeof(codes_suback[0]));
	test_rc(rc, "PktSubAck");

	return 0;
}

