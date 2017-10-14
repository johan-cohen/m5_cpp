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

#include "PktConnect.hpp"
#include <stdexcept>
#include <cstring>

namespace  m5 {

uint8_t PktConnect::headerFlags(void) const
{
	uint8_t f;

	f = (cleanStart << 0x01) +
	    ((willMsg != nullptr && willMsg->length() > 0) ? (0x01 << 2) : 0) +
	    ((willQoS & 0x03) << 3) +
	    (willRetain == 1 ? (1 << 5) : 0) +
	    ((password != nullptr && password->length() > 0) ? (0x01 << 6) : 0) +
	    ((userName != nullptr && userName->length() > 0) ? (0x01 << 7) : 0);

	return f;
}

uint32_t PktConnect::payloadWireSize(void) const
{
	uint32_t wireSize;

	if (clientId == nullptr || clientId->length() < clientIdMinLen ||
	    clientId->length() > clientIdMaxLen) {
		throw std::invalid_argument("Invalid ClientId");
	}

	wireSize = stringLenSize  + clientId->length();

	if (willMsg != nullptr && willTopic != nullptr) {
		if (willMsg->length() > 0 && willTopic->length() > 0) {
			wireSize += stringLenSize + willTopic->length() +
				    binaryLenSize + willMsg->length();
		} else if (willMsg->length() + willTopic->length() != 0) {
			throw std::invalid_argument("Invalid Will Msg or Topic");
		}
	}


	if (userName != nullptr && userName->length() > 0) {
		wireSize += stringLenSize + userName->length();
	}

	if (password != nullptr && password->length() > 0) {
		wireSize += binaryLenSize + password->length();
	}

	return wireSize;
}

uint32_t PktConnect::propertiesWireSize(void) const
{
	/* xxx */
	return 0;
}

void PktConnect::writePayload(AppBuf &buf)
{
	buf.writeBinary(clientId->rawData(), clientId->length());

	if (willMsg != nullptr && willMsg->length() > 0) {
		buf.writeBinary(willTopic->rawData(), willTopic->length());
		buf.writeBinary(willMsg->rawData(), willMsg->length());
	}

	if (userName != nullptr && userName->length() > 0) {
		buf.writeBinary(userName->rawData(), userName->length());
	}

	if (password != nullptr && password->length() > 0) {
		buf.writeBinary(password->rawData(), password->length());
	}
}

void PktConnect::writeProperties(AppBuf &buf, uint32_t propWireSize)
{
	(void)propWireSize;

	/* xxx So far, propWS is zero */
	buf.writeVBI(0);
}

void PktConnect::init(const uint8_t *clientId, uint16_t len, bool cleanStart)
{
	this->setClientId(clientId, len);
	this->setCleanStart(cleanStart);
}

PktConnect::PktConnect(const uint8_t *clientId, uint16_t len, bool cleanStart)
{
	init(clientId, len, cleanStart);
}

PktConnect::PktConnect(const char *clientId, bool cleanStart)
{
	init((const uint8_t *)clientId, strlen(clientId), cleanStart);
}

PktConnect::~PktConnect()
{
	delete clientId;
	delete willTopic;
	delete willMsg;
	delete userName;
	delete password;
}

uint32_t PktConnect::writeTo(AppBuf &buf)
{
	uint32_t fullPktSize;
	uint32_t payloadWS;
	uint32_t propWSWS;
	uint32_t propWS;
	uint32_t remLenWS;
	uint32_t remLen;

	payloadWS = payloadWireSize();
	propWS = propertiesWireSize();
	propWSWS = VBIWireSize(propWS);

	remLen = 1 + 2 + 4 + 1 + 2 + propWSWS + propWS + payloadWS;
	remLenWS = VBIWireSize(remLen);

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	buf.writeNum8((uint8_t)PktType::CONNECT << 4);
	buf.writeVBI(remLen);
	buf.writeString(protocolStr);
	buf.writeNum8(protocolVersion5);
	buf.writeNum8(headerFlags());
	buf.writeNum16(this->keepAlive);

	writeProperties(buf, propWS);
	writePayload(buf);

	return fullPktSize;
}

uint32_t PktConnect::readFrom(AppBuf &buf)
{
	(void)buf;

	/* xxx */

	return 0;
}

uint32_t PktConnect::getId(void) const
{
	return (uint32_t)PktType::CONNECT;
}

void PktConnect::setClientId(const uint8_t *data, uint16_t size)
{
	this->clientId = new AppBuf(data, size);
}

void PktConnect::setClientId(const char *str)
{
	this->setClientId((const uint8_t *)str, strlen(str));
}

void PktConnect::setWillTopic(const uint8_t *data, uint16_t size)
{
	this->willTopic = new AppBuf(data, size);
}

void PktConnect::setWillTopic(const char *str)
{
	this->setWillTopic((const uint8_t *)str, strlen(str));
}

void PktConnect::setWillMsg(const uint8_t *data, uint16_t size)
{
	this->willMsg = new AppBuf(data, size);
}

void PktConnect::setWillMsg(const char *str)
{
	this->setWillMsg((const uint8_t *)str, strlen(str));
}

void PktConnect::setUserName(const uint8_t *data, uint16_t size)
{
	this->userName = new AppBuf(data, size);
}

void PktConnect::setUserName(const char *str)
{
	this->setUserName((const uint8_t *)str, strlen(str));
}

void PktConnect::setPassword(const uint8_t *data, uint16_t size)
{
	this->password = new AppBuf(data, size);
}

void PktConnect::setPassword(const char *str)
{
	this->setPassword((const uint8_t *)str, strlen(str));
}

void PktConnect::setKeyAlive(uint16_t keepAlive)
{
	this->keepAlive = keepAlive;
}

void PktConnect::setWillRetain(bool willRetain)
{
	this->willRetain = (willRetain ? 1 : 0);
}

void PktConnect::setWillQoS(enum PktQoS qos)
{
	this->willQoS = (uint8_t)qos;
}

void PktConnect::setCleanStart(bool cleanStart)
{
	this->cleanStart = (cleanStart ? 1 : 0);
}

}
