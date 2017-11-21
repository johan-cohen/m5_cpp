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
 * Tests for the MQTT 5 Low Level Packet Library
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
#include "PktConnect.hpp"

#include <cstring>

namespace m5 {
	bool operator==(const PktConnect &a, const PktConnect &b) {
		if (a.keepAlive() != b.keepAlive()) {
			return false;
		}

		if (a.willRetain() != b.willRetain()) {
			return false;
		}

		if (a.willQoS() != b.willQoS()) {
			return false;
		}

		if (a.cleanStart() != b.cleanStart()) {
			return false;
		}

		if (a.clientId() != b.clientId()) {
			return false;
		}

		if (a.willTopic() != b.willTopic()) {
			return false;
		}

		if (a.willMsg() != b.willMsg()) {
			return false;
		}

		if (a.userName() != b.userName()) {
			return false;
		}

		if (a.password() != b.password()) {
			return false;
		}

		return true;
	}

	bool operator!=(const PktConnect &a, const PktConnect &b) {
		return !(a == b);
	}
}

int test(void)
{
	const uint8_t protocolNameStr[] = {0, 4, 'M', 'Q', 'T', 'T'};
	const char clientId[] = "m5";
	const char willTopic[] = "msgs";
	const char willMsg[] = "Bye!";
	const char userName[] = "name";
	const char password[] = "pass";
	const uint16_t keepAlive = 0xABCD;

	m5::PktConnect *connect;
	uint8_t remLenWireSize;
	uint8_t bigString[64];
	uint32_t fullPktSize;
	uint32_t remLen;
	m5::AppBuf *buf;
	uint32_t bytes;
	uint8_t vbiWS;
	uint32_t vbi;
	m5::StatusCode rc;

	buf = new m5::AppBuf(128);
	connect = new m5::PktConnect("m5_client");

	connect->cleanStart(true);
	connect->clientId(clientId);
	connect->keepAlive(keepAlive);
	connect->userName(userName);
	connect->password(password);
	connect->will(willTopic, willMsg);
	connect->willQoS(m5::PktQoS::QoS2);
	connect->willRetain(true);

	/* RemLen: Variable Header min size: 10 + 1, plus the payload */
	remLen = 10 + 1 +
		 2 + strlen(clientId) + 2 + strlen(willTopic) +
		 2 + strlen(willMsg) + 2 + strlen(userName) +
		 2 + strlen(password);
	remLenWireSize = m5::VBIWireSize(remLen);
	fullPktSize = 1 + remLenWireSize + remLen;

	bytes = connect->writeTo(*buf);
	if (bytes == 0 || connect->status() != m5::StatusCode::SUCCESS) {
		error_exit("writeTo");
	}

	if (buf->length() != fullPktSize) {
		error_exit("writeTo: buffer length");
	}

	if (buf->readNum8() != m5::firstByte(m5::PktType::CONNECT)) {
		error_exit("writeTo: packet type");
	}

	rc = buf->readVBI(vbi, vbiWS);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("writeTo: Remaining Length");
	}

	if (remLen != vbi || m5::VBIWireSize(remLen) != vbiWS) {
		error_exit("writeTo: Remaining Length");
	}

	buf->read(bigString, sizeof(protocolNameStr));
	if (memcmp(bigString, protocolNameStr, sizeof(protocolNameStr)) != 0) {
		error_exit("writeTo: Protocol Name");
	}

	if (buf->readNum8() != 5) {
		error_exit("writeTo: Protocol Version");
	}

	if (buf->readNum8() != 0xF6) {
		error_exit("writeTo: Control Flags");
	}

	if (buf->readNum16() != keepAlive) {
		error_exit("writeTo: Keep Alive");
	}

	rc = buf->readVBI(vbi, vbiWS);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("writeTo: Properties Length");
	}

	if (vbi != 0 || vbiWS != 1) {
		error_exit("writeTo: Properties Length");
	}

	m5::ByteArray str;
	rc = buf->readBinary(str);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("readBinary");
	}
	if (str.size() != strlen(clientId) || memcmp(str.data(), clientId, strlen(clientId)) != 0) {
		error_exit("writeTo: clientId");
	}

	rc = buf->readBinary(str);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("readBinary");
	}
	if (str.size() != strlen(willTopic) || memcmp(str.data(), willTopic, strlen(willTopic)) != 0) {
		error_exit("writeTo: willTopic");
	}

	rc = buf->readBinary(str);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("readBinary");
	}
	if (str.size() != strlen(willMsg) || memcmp(str.data(), willMsg, strlen(willMsg)) != 0) {
		error_exit("writeTo: willMsg");
	}

	rc = buf->readBinary(str);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("readBinary");
	}
	if (str.size() != strlen(userName) || memcmp(str.data(), userName, strlen(userName)) != 0) {
		error_exit("writeTo: userName");
	}

	rc = buf->readBinary(str);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("readBinary");
	}
	if (str.size() != strlen(password) || memcmp(str.data(), password, strlen(password)) != 0) {
		error_exit("writeTo: password");
	}

	buf->rewind();

	m5::PktConnect *connectRead = new m5::PktConnect(*buf);
	if (connectRead->status() != m5::StatusCode::SUCCESS) {
		error_exit("PktConnect constructor read");
	}
	if (*connect != *connectRead) {
		error_exit("PktConnect constructor");
	}
	delete connectRead;

	delete connect;
	delete buf;

	return 0;
}

int testProperties(void)
{
	const char clientId[] = "m5_client";
	const char str[] = "World!";
	uint32_t u32 = 0xABCDEF01;
	uint16_t u16 = 0xABCD;
	m5::AppBuf buf(256);

	m5::PktConnect *connectRead;
	m5::PktConnect *connect;

	connect = new m5::PktConnect(clientId);

	connect->sessionExpiryInterval(u32);
	connect->authenticationMethod(str);
	connect->authenticationData((const uint8_t *)str, strlen(str));
	connect->requestProblemInformation(true);
	connect->willDelayInterval(u32);
	connect->requestResponseInformation(true);
	connect->receiveMaximum(u16);
	connect->topicAliasMaximum(u16);
	connect->userProperty(str, str);
	connect->userProperty(str, str);
	connect->userProperty(str, str);
	connect->maximumPacketSize(u32);

	connect->writeTo(buf);

	connectRead = new m5::PktConnect(buf);

	if (connectRead->sessionExpiryInterval() != u32) {
		error_exit("properties: sessionExpiryInterval");
	}

	if (connectRead->requestProblemInformation() != true) {
		error_exit("properties: requestProblemInformation");
	}

	if (connectRead->willDelayInterval() != u32) {
		error_exit("properties: willDelayInterval");
	}

	if (connectRead->requestResponseInformation() != true) {
		error_exit("properties: ByteArray");
	}

	if (connectRead->receiveMaximum() != u16) {
		error_exit("properties: receiveMaximum");
	}

	if (connectRead->topicAliasMaximum() != u16) {
		error_exit("properties: topicAliasMaximum");
	}

	if (connectRead->maximumPacketSize() != u32) {
		error_exit("properties: maximumPacketSize");
	}

	delete connectRead;
	delete connect;

	return 0;
}

int main(void)
{
	int rc;

	rc = test();
	test_rc(rc, "PktConnect");

	rc = testProperties();
	test_rc(rc, "PktConnect Properties");

	return 0;
}

