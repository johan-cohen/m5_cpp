/*
 *                                   1byt3
 *
 *                              License Notice
 *
 * 1byt3 provides a commercial license agreement for this software. This
 * commercial license can be used for development of proprietary/commercial
 * software. Under this commercial license you do not need to comply with the
 * terms of the GNU Affero General Public License, either version 3 of the
 * License, or (at your option) any later version.
 *
 * If you don't receive a commercial license from us (1byt3), you MUST assume
 * that this software is distributed under the GNU Affero General Public
 * License, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Contact us for additional information: customers at 1byt3.com
 *
 *                          End of License Notice
 */

/*
 * MQTT 5 Low Level Packet Library
 *
 * Copyright (C) 2017 1byt3, customers at 1byt3.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "test_Common.hpp"
#include "PktConnAck.hpp"

#include <cstring>

int test(void)
{
	const char msg[] = "Hello, World!";
	m5::ByteArray msgArray(msg, msg + strlen(msg));
	uint32_t u32 = 0xABCDEF01;
	uint16_t u16 = 0xABCD;
	m5::PktConnAck *connAck;
	m5::AppBuf buf(256);

	connAck = new m5::PktConnAck(false, m5::ReasonCode::SUCCESS);
	connAck->receiveMaximum(u16);
	connAck->maximumQoS(m5::PktQoS::QoS2);
	connAck->retainAvailable(true);
	connAck->maximumPacketSize(u32);
	connAck->assignedClientIdentifier(msg);
	connAck->topicAliasMaximum(u16);
	connAck->reasonString(msg);
	connAck->userProperty(msg, msg);
	connAck->wildcardSubscriptionAvailable(true);
	connAck->sharedSubscriptionAvailable(true);
	connAck->serverKeepAlive(u16);
	connAck->responseInformation(msg);
	connAck->serverReference(msg);
	connAck->authenticationMethod(msg);
	connAck->authenticationData((const uint8_t *)msg, strlen(msg));

	connAck->writeTo(buf);

	m5::PktConnAck connAckRead;

	connAckRead.readFrom(buf);
	if (connAckRead.receiveMaximum() != u16) {
		throw std::logic_error("receiveMaximum read/write");
	}
	if (connAckRead.maximumQoS() != m5::PktQoS::QoS2) {
		throw std::logic_error("maximumQoS read/write");
	}
	if (connAckRead.retainAvailable() != true) {
		throw std::logic_error("retainAvailable read/write");
	}
	if (connAckRead.maximumPacketSize() != u32) {
		throw std::logic_error("maximumPacketSize read/write");
	}
	if (connAckRead.assignedClientIdentifier() != msgArray) {
		throw std::logic_error("assignedClientIdentifier read/write");
	}
	if (connAckRead.topicAliasMaximum() != u16) {
		throw std::logic_error("topicAliasMaximum read/write");
	}
	if (connAckRead.reasonString() != msgArray) {
		throw std::logic_error("reasonString read/write");
	}
	if (connAckRead.wildcardSubscriptionAvailable() != true) {
		throw std::logic_error("wildcardSubscriptionAvailable read/write");
	}
	if (connAckRead.sharedSubscriptionAvailable() != true) {
		throw std::logic_error("sharedSubscriptionAvailable read/write");
	}
	if (connAckRead.serverKeepAlive() != u16) {
		throw std::logic_error("serverKeepAlive read/write");
	}
	if (connAckRead.responseInformation() != msgArray) {
		throw std::logic_error("responseInformation read/write");
	}
	if (connAckRead.serverReference() != msgArray) {
		throw std::logic_error("serverReference read/write");
	}
	if (connAckRead.authenticationMethod() != msgArray) {
		throw std::logic_error("authenticationMethod read/write");
	}
	if (connAckRead.authenticationData() != msgArray) {
		throw std::logic_error("authenticationData read/write");
	}

	delete connAck;

	return 0;
}

int main(void)
{
	int rc;

	rc = test();
	test_rc(rc, "PktConnAck");

	return 0;
}
