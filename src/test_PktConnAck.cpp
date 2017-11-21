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
	m5::StatusCode rc;

	connAck = new m5::PktConnAck(false, m5::ReasonCode::SUCCESS);

	rc = connAck->receiveMaximum(u16);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("receiveMaximum");
	}

	rc = connAck->maximumQoS(m5::PktQoS::QoS2);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("maximumQoS");
	}

	rc = connAck->retainAvailable(true);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("retainAvailable");
	}

	rc = connAck->maximumPacketSize(u32);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("maximumPacketSize");
	}

	rc = connAck->assignedClientIdentifier(msg);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("assignedClientIdentifier");
	}

	rc = connAck->topicAliasMaximum(u16);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("topicAliasMaximum");
	}

	rc = connAck->reasonString(msg);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("reasonString");
	}

	rc = connAck->userProperty(msg, msg);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("userProperty");
	}

	rc = connAck->wildcardSubscriptionAvailable(true);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("wildcardSubscriptionAvailable");
	}

	rc = connAck->sharedSubscriptionAvailable(true);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("sharedSubscriptionAvailable");
	}

	rc = connAck->serverKeepAlive(u16);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("serverKeepAlive");
	}

	rc = connAck->responseInformation(msg);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("responseInformation");
	}

	rc = connAck->serverReference(msg);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("serverReference");
	}

	rc = connAck->authenticationMethod(msg);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("authenticationMethod");
	}

	rc = connAck->authenticationData((const uint8_t *)msg, strlen(msg));
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("authenticationData");
	}

	connAck->writeTo(buf);
	if (connAck->status() != m5::StatusCode::SUCCESS) {
		error_exit("writeTo");
	}

	m5::PktConnAck connAckRead;

	connAckRead.readFrom(buf);
	if (connAckRead.status() != m5::StatusCode::SUCCESS) {
		error_exit("readFrom");
	}

	if (connAckRead.receiveMaximum() != u16) {
		error_exit("receiveMaximum read/write");
	}

	if (connAckRead.maximumQoS() != m5::PktQoS::QoS2) {
		error_exit("maximumQoS read/write");
	}

	if (connAckRead.retainAvailable() != true) {
		error_exit("retainAvailable read/write");
	}

	if (connAckRead.maximumPacketSize() != u32) {
		error_exit("maximumPacketSize read/write");
	}

	if (connAckRead.assignedClientIdentifier() != msgArray) {
		error_exit("assignedClientIdentifier read/write");
	}

	if (connAckRead.topicAliasMaximum() != u16) {
		error_exit("topicAliasMaximum read/write");
	}

	if (connAckRead.reasonString() != msgArray) {
		error_exit("reasonString read/write");
	}

	if (connAckRead.wildcardSubscriptionAvailable() != true) {
		error_exit("wildcardSubscriptionAvailable read/write");
	}

	if (connAckRead.sharedSubscriptionAvailable() != true) {
		error_exit("sharedSubscriptionAvailable read/write");
	}

	if (connAckRead.serverKeepAlive() != u16) {
		error_exit("serverKeepAlive read/write");
	}

	if (connAckRead.responseInformation() != msgArray) {
		error_exit("responseInformation read/write");
	}

	if (connAckRead.serverReference() != msgArray) {
		error_exit("serverReference read/write");
	}

	if (connAckRead.authenticationMethod() != msgArray) {
		error_exit("authenticationMethod read/write");
	}

	if (connAckRead.authenticationData() != msgArray) {
		error_exit("authenticationData read/write");
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
