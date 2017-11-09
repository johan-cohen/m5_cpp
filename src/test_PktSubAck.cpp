
#include "test_Common.hpp"
#include "PktSubAck.hpp"

int test(void)
{
	m5::PktSubAck *subAck;
	const char msg[] = "Hello, World!";
	m5::AppBuf buf(256);

	subAck = new m5::PktSubAck();
	subAck->packetId(0xABCD);
	subAck->append(m5::ReasonCode::GRANTED_QOS0);
	subAck->append(m5::ReasonCode::GRANTED_QOS1);
	subAck->append(m5::ReasonCode::GRANTED_QOS2);
	subAck->append(m5::ReasonCode::UNSPECIFIED_ERROR);
	subAck->append(m5::ReasonCode::IMPLEMENTATION_SPECIFIC_ERROR);
	subAck->append(m5::ReasonCode::NOT_AUTHORIZED);
	subAck->append(m5::ReasonCode::TOPIC_FILTER_INVALID);
	subAck->append(m5::ReasonCode::PACKET_IDENTIFIER_IN_USE);
	subAck->append(m5::ReasonCode::QUOTA_EXCEEDED);
	subAck->append(m5::ReasonCode::SHARED_SUBSCRIPTION_NOT_SUPPORTED);
	subAck->append(m5::ReasonCode::SUBSCRIPTION_IDENTIFIERS_NOT_SUPPORTED);
	subAck->properties.userProperty(msg, msg);
	subAck->properties.reasonString(msg);
	subAck->properties.userProperty(msg, msg);
	subAck->writeTo(buf);

	m5::PktSubAck subAckRead(buf);

	if (subAck->packetId() != subAckRead.packetId()) {
		throw std::logic_error("read/write: Packet Id");
	}
	if (subAck->reasonCodes() != subAckRead.reasonCodes()) {
		throw std::logic_error("read/write: Reason Codes");
	}

	delete subAck;

	return 0;
}

int main(void)
{
	int rc;

	rc = test();
	test_rc(rc, "PktSubAck");

	return 0;
}

