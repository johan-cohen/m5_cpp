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

#include "PktConnect.hpp"
#include "test_Common.hpp"

#include <stdexcept>
#include <cstring>

namespace m5 {
	bool operator==(const AppBuf &a, const AppBuf &b) {
		if (a.length() != b.length()) {
			return false;
		}

		if (memcmp(a.rawData(), b.rawData(), a.length()) != 0) {
			return false;
		}

		return true;
	}

	bool operator!=(const AppBuf &a, const AppBuf &b) {
		return !(a == b);
	}

	bool operator==(const PktConnect &a, const PktConnect &b) {
		if (a.getKeepAlive() != b.getKeepAlive()) {
			return false;
		}

		if (a.getWillRetain() != b.getWillRetain()) {
			return false;
		}

		if (a.getWillQoS() != b.getWillQoS()) {
			return false;
		}

		if (a.getCleanStart() != b.getCleanStart()) {
			return false;
		}

		if (*a.clientId != *b.clientId) {
			return false;
		}

		if (*a.willTopic != *b.willTopic) {
			return false;
		}

		if (*a.willMsg != *b.willMsg) {
			return false;
		}

		if (*a.userName != *b.userName) {
			return false;
		}

		if (*a.password != *b.password) {
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
	uint16_t len;

	buf = new m5::AppBuf(128);
	connect = new m5::PktConnect("m5_client");

	connect->setCleanStart(true);
	/* this must call the clientId destructor and create a new obj */
	connect->setClientId(clientId);
	connect->setKeyAlive(keepAlive);
	connect->setUserName(userName);
	connect->setPassword(password);
	connect->setWill(willTopic, willMsg);
	connect->setWillQoS(m5::PktQoS::QoS2);
	connect->setWillRetain(true);

	/* RemLen: Variable Header min size: 10 + 1, plus the payload */
	remLen = 10 + 1 +
		 2 + strlen(clientId) + 2 + strlen(willTopic) +
		 2 + strlen(willMsg) + 2 + strlen(userName) +
		 2 + strlen(password);
	remLenWireSize = m5::VBIWireSize(remLen);
	fullPktSize = 1 + remLenWireSize + remLen;

	connect->writeTo(*buf);

	if (buf->length() != fullPktSize) {
		throw std::logic_error("writeTo");
	}

	if (buf->readNum8() != ((uint8_t)m5::PktType::CONNECT << 4)) {
		throw std::logic_error("writeTo: PktType");
	}

	if (remLen != buf->readVBI()) {
		throw std::logic_error("writeTo: Remaining Length");
	}

	buf->read(bigString, sizeof(protocolNameStr));
	if (memcmp(bigString, protocolNameStr, sizeof(protocolNameStr)) != 0) {
		throw std::logic_error("writeTo: Protocol Name");
	}

	if (buf->readNum8() != 5) {
		throw std::logic_error("writeTo: Protocol Version");
	}

	if (buf->readNum8() != 0xF6) {
		throw std::logic_error("writeTo: Control Flags");
	}

	if (buf->readNum16() != keepAlive) {
		throw std::logic_error("writeTo: Control Flags");
	}

	if (buf->readVBI() != 0) {
		throw std::logic_error("writeTo: Properties Length");
	}

	buf->readBinary(bigString, len, strlen(clientId));
	if (memcmp(bigString, clientId, strlen(clientId)) != 0) {
		throw std::logic_error("writeTo: clientId");
	}

	buf->readBinary(bigString, len, strlen(willTopic));
	if (memcmp(bigString, willTopic, strlen(willTopic)) != 0) {
		throw std::logic_error("writeTo: willTopic");
	}

	buf->readBinary(bigString, len, strlen(willMsg));
	if (memcmp(bigString, willMsg, strlen(willMsg)) != 0) {
		throw std::logic_error("writeTo: willMsg");
	}

	buf->readBinary(bigString, len, strlen(userName));
	if (memcmp(bigString, userName, strlen(userName)) != 0) {
		throw std::logic_error("writeTo: userName");
	}

	buf->readBinary(bigString, len, strlen(password));
	if (memcmp(bigString, password, strlen(password)) != 0) {
		throw std::logic_error("writeTo: password");
	}

	buf->rewind();

	m5::PktConnect *connectRead = new m5::PktConnect(*buf);

	if (*connect != *connectRead) {
		throw std::logic_error("PktConnect constructor");
	}

	delete connectRead;

	delete connect;
	delete buf;

	return 0;
}

int main(void)
{
	int rc = test();
	test_rc(rc, "PktConnect");

	return 0;
}
