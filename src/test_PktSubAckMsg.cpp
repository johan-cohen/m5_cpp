
#include "test_Common.hpp"
#include "PktUnsubAck.hpp"
#include "PktSubAck.hpp"

template <typename T>
int test(m5::ReasonCode *codes, std::size_t size)
{
	const char msg[] = "Hello, World!";
	m5::AppBuf buf(256);
	m5::StatusCode sc;
	T *pkt;

	pkt = new T();
	pkt->packetId(0xABCD);

	for (std::size_t i = 0; i < size; i++) {
		pkt->append(codes[i]);
	}

	sc = pkt->userProperty(msg, msg);
	if (sc != m5::StatusCode::SUCCESS) {
		error_exit("userProperty");
	}

	sc = pkt->reasonString(msg);
	if (sc != m5::StatusCode::SUCCESS) {
		error_exit("userProperty");
	}

	sc = pkt->userProperty(msg, msg);
	if (sc != m5::StatusCode::SUCCESS) {
		error_exit("userProperty");
	}

	pkt->writeTo(buf);
	if (pkt->status() != m5::StatusCode::SUCCESS) {
		error_exit("write");
	}

	T pktRead(buf);
	if (pktRead.status() != m5::StatusCode::SUCCESS) {
		error_exit("readFrom");
	}

	if (pkt->packetId() != pktRead.packetId()) {
		error_exit("read/write: Packet Id");
	}
	if (pkt->reasonCodes() != pktRead.reasonCodes()) {
		error_exit("read/write: Reason Codes");
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
	m5::ReasonCode codes_unsuback[] = {
		m5::ReasonCode::SUCCESS,
		m5::ReasonCode::NO_SUBSCRIPTION_EXISTED,
		m5::ReasonCode::UNSPECIFIED_ERROR,
		m5::ReasonCode::IMPLEMENTATION_SPECIFIC_ERROR,
		m5::ReasonCode::NOT_AUTHORIZED,
		m5::ReasonCode::TOPIC_FILTER_INVALID,
		m5::ReasonCode::PACKET_IDENTIFIER_IN_USE };
	int rc;

	rc = test<m5::PktSubAck>(codes_suback, sizeof(codes_suback) / sizeof(codes_suback[0]));
	test_rc(rc, "PktSubAck");

	rc = test<m5::PktUnsubAck>(codes_unsuback, sizeof(codes_unsuback) / sizeof(codes_unsuback[0]));
	test_rc(rc, "PktUnsubAck");

	return 0;
}

