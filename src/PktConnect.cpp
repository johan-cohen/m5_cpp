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

static const uint32_t clientIdMinLen = 1;
static const uint32_t clientIdMaxLen = 23;

static const uint8_t protocolVersion5 = 0x05;

static const char protocolStr[] = "MQTT";
static const uint8_t protocolNameStr[] = {0, 4, 'M', 'Q', 'T', 'T'};
static const std::size_t protocolNameStrLen = sizeof(protocolNameStr);

/* Protocol Name (2 + 4), Protocol Level (1), Conn Flags (1), Keep Alive (2) */
static const uint32_t connectVarHdrMinSize = 10;
static const uint32_t connectPktMinSize = 2 + connectVarHdrMinSize;

uint8_t PktConnect::packConnectFlags(void)
{
	uint8_t flags;

	flags = (cleanStart << 0x01);
	flags += ((willQoS & 0x03) << 3) + (willRetain == 1 ? (1 << 5) : 0);

	if (willMsg != nullptr) {
		flags += (willMsg->length() > 0 ? (0x01 << 2) : 0);
	}

	if (password != nullptr) {
		flags += (password->length() > 0 ? (0x01 << 6) : 0);
	}

	if (userName != nullptr) {
		flags += (userName->length() > 0 ? (0x01 << 7) : 0);
	}

	return flags;
}

bool PktConnect::flagCleanStart(uint8_t flags)
{
	return flags & (1 << 1);
}

bool PktConnect::flagWillMsg(uint8_t flags)
{
	return flags & (1 << 2);
}

uint8_t PktConnect::flagWillQoS(uint8_t flags)
{
	return (flags & (3 << 3)) >> 3;
}

bool PktConnect::flagWillRetain(uint8_t flags)
{
	return flags & (1 << 4);
}

bool PktConnect::flagPassword(uint8_t flags)
{
	return flags & (1 << 5);
}

bool PktConnect::flagUserName(uint8_t flags)
{
	return flags & (1 << 6);
}

uint32_t PktConnect::payloadWireSize(void) const
{
	uint32_t wireSize;

	if (clientId == nullptr) {
		throw std::invalid_argument("Invalid ClientId");
	}

	wireSize = stringLenSize  + clientId->length();

	if (willMsg != nullptr && willMsg->length() > 0 &&
	    willTopic != nullptr && willTopic->length() > 0) {
		wireSize += stringLenSize + willTopic->length();
		wireSize += binaryLenSize + willMsg->length();
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

PktConnect::PktConnect(AppBuf &buf)
{
	this->readFrom(buf);
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
	buf.writeNum8(packConnectFlags());
	buf.writeNum16(this->keepAlive);

	writeProperties(buf, propWS);
	writePayload(buf);

	return fullPktSize;
}

uint32_t PktConnect::readFrom(AppBuf &buf)
{
	std::size_t alreadyTraversed = buf.traversed();
	uint8_t connectFlags;
	uint8_t remLenWS;
	uint32_t propLen;
	uint32_t remLen;
	uint8_t first;

	if (buf.bytesToRead() < m5::connectPktMinSize) {
		throw std::out_of_range("No enough space in input buffer");
	}

	first = buf.readNum8();
	if (first != ((uint8_t)PktType::CONNECT << 4)) {
		throw std::invalid_argument("CONNECT msg not found in buf");
	}

	buf.readVBI(remLen, remLenWS);

	/* Add 1 to allow the min property length wire size to be 1 byte
	 * Add the Client Id min len: 2 for field length and clientIdMinLen for
	 * the content.
	 */
	if (remLen < m5::connectVarHdrMinSize + 1 + 2 + m5::clientIdMinLen) {
		throw std::out_of_range("No enough space in input buffer");
	}

	const uint8_t *ptr = buf.rawData() + buf.traversed();
	if (memcmp(ptr, m5::protocolNameStr, m5::protocolNameStrLen) != 0) {
		throw std::invalid_argument("Invalid protocol name string");
	}
	buf.readSkip(m5::protocolNameStrLen);

	if (buf.readNum8() != protocolVersion5) {
		throw std::invalid_argument("Invalid protocol version");
	}

	connectFlags = buf.readNum8();
	this->cleanStart = flagCleanStart(connectFlags);
	this->willRetain = flagWillRetain(connectFlags);
	this->willQoS = flagWillQoS(connectFlags);

	keepAlive = buf.readNum16();

	propLen = buf.readVBI();
	/* xxx Implement properties reading */
	buf.readSkip(propLen);

	this->clientId = AppBuf::createFrom(buf, buf.readNum16());

	if (flagWillMsg(connectFlags) == true) {
		this->willTopic = AppBuf::createFrom(buf, buf.readNum16());
		this->willMsg = AppBuf::createFrom(buf, buf.readNum16());
	}

	if (flagUserName(connectFlags) == true) {
		this->userName = AppBuf::createFrom(buf, buf.readNum16());
	}

	if (flagPassword(connectFlags) == true) {
		this->password = AppBuf::createFrom(buf, buf.readNum16());
	}

	if (buf.traversed() - alreadyTraversed != 1 + remLenWS + remLen) {
		throw std::invalid_argument("Corrupted input buffer");
	}

	return 0;
}

uint32_t PktConnect::getId(void) const
{
	return (uint32_t)PktType::CONNECT;
}

void PktConnect::setClientId(const uint8_t *data, uint16_t size)
{
	if (this->clientId != nullptr) {
		delete this->clientId;
	}

	if (size < clientIdMinLen || size > clientIdMaxLen) {
			throw std::invalid_argument("Invalid ClientId length");
	}

	this->clientId = new AppBuf(data, size);
}

void PktConnect::setClientId(const char *str)
{
	this->setClientId((const uint8_t *)str, strlen(str));
}

void PktConnect::setWill(const uint8_t *topic, uint16_t topic_size,
			 const uint8_t *msg, uint16_t msg_size)
{
	if (topic_size == 0 || msg_size == 0) {
		throw std::invalid_argument("Invalid Will Topic or Msg length");
	}

	this->willTopic = new AppBuf(topic, topic_size);
	this->willMsg = new AppBuf(msg, msg_size);
}

void PktConnect::setWill(const char *topic, const char *msg)
{
	this->setWill((const uint8_t *)topic, strlen(topic),
		      (const uint8_t *)msg, strlen(msg));
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

