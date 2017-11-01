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

	flags = (cleanStart() << 0x01);
	flags += (willMsg().size() > 0 ? (0x01 << 2) : 0);
	flags += ((_willQoS & 0x03) << 3);
	flags += (willRetain() == 1 ? (1 << 5) : 0);
	flags += (password().size() > 0 ? (0x01 << 6) : 0);
	flags += (userName().size() > 0 ? (0x01 << 7) : 0);

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

PktQoS PktConnect::flagWillQoS(uint8_t flags)
{
	return (PktQoS)((flags & (3 << 3)) >> 3);
}

bool PktConnect::flagWillRetain(uint8_t flags)
{
	return flags & (1 << 5);
}

bool PktConnect::flagPassword(uint8_t flags)
{
	return flags & (1 << 6);
}

bool PktConnect::flagUserName(uint8_t flags)
{
	return flags & (1 << 7);
}

uint32_t PktConnect::payloadWireSize(void) const
{
	uint32_t wireSize;

	wireSize = stringLenSize  + clientId().size();

	if (willMsg().size() > 0 && willTopic().size() > 0) {
		wireSize += stringLenSize + willTopic().size();
		wireSize += binaryLenSize + willMsg().size();
	}


	if (userName().size() > 0) {
		wireSize += stringLenSize + userName().size();
	}

	if (password().size() > 0) {
		wireSize += binaryLenSize + password().size();
	}

	return wireSize;
}

void PktConnect::writePayload(AppBuf &buf)
{
	buf.writeBinary(&clientId()[0], clientId().size());

	if (willMsg().size() > 0) {
		buf.writeBinary(&willTopic()[0], willTopic().size());
		buf.writeBinary(&willMsg()[0], willMsg().size());
	}

	if (userName().size() > 0) {
		buf.writeBinary(&userName()[0], userName().size());
	}

	if (password().size() > 0) {
		buf.writeBinary(&password()[0], password().size());
	}
}

void PktConnect::init(const uint8_t *clientId, uint16_t len, bool cleanStart)
{
	this->clientId(clientId, len);
	this->cleanStart(cleanStart);
}

PktConnect::PktConnect(AppBuf &buf) : properties(PktType::CONNECT)
{
	this->readFrom(buf);
}

PktConnect::PktConnect(const uint8_t *clientId, uint16_t len, bool cleanStart) :
		       properties(PktType::CONNECT)
{
	init(clientId, len, cleanStart);
}

PktConnect::PktConnect(const char *clientId, bool cleanStart) :
		       properties(PktType::CONNECT)
{
	init((const uint8_t *)clientId, strlen(clientId), cleanStart);
}

PktConnect::~PktConnect()
{
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
	propWS = properties.wireSize();
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
	buf.writeNum16(keepAlive());
	properties.write(buf);
	writePayload(buf);

	return fullPktSize;
}

/* xxx remove this later */
void setVector(std::vector<uint8_t> &v, AppBuf &buf)
{
	if (buf.bytesToRead() < 2) {
		throw std::out_of_range("No enough space in input buffer");
	}

	auto itemLen = buf.readNum16();
	if (buf.bytesToRead() < itemLen) {
		throw std::out_of_range("No enough space in input buffer");
	}

	v.assign(buf.currentRead(), buf.currentRead() + itemLen);
	buf.readSkip(itemLen);
}

uint32_t PktConnect::readFrom(AppBuf &buf)
{
	std::size_t alreadyTraversed = buf.traversed();
	uint8_t connectFlags;
	uint8_t remLenWS;
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

	const uint8_t *ptr = buf.data() + buf.traversed();
	if (memcmp(ptr, m5::protocolNameStr, m5::protocolNameStrLen) != 0) {
		throw std::invalid_argument("Invalid protocol name string");
	}
	buf.readSkip(m5::protocolNameStrLen);

	if (buf.readNum8() != protocolVersion5) {
		throw std::invalid_argument("Invalid protocol version");
	}

	connectFlags = buf.readNum8();
	this->cleanStart(flagCleanStart(connectFlags));
	this->willRetain(flagWillRetain(connectFlags));
	this->willQoS(flagWillQoS(connectFlags));

	keepAlive(buf.readNum16());

	properties.read(buf);

	setVector(this->_clientId, buf);
	if (flagWillMsg(connectFlags) == true) {
		setVector(this->_willTopic, buf);
		setVector(this->_willMsg, buf);
	}

	if (flagUserName(connectFlags) == true) {
		setVector(this->_userName, buf);
	}

	if (flagPassword(connectFlags) == true) {
		setVector(this->_password, buf);
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

void PktConnect::clientId(const uint8_t *data, uint16_t size)
{
	if (size < clientIdMinLen || size > clientIdMaxLen) {
			throw std::invalid_argument("Invalid ClientId length");
	}

	this->_clientId.assign(data, data + size);
}

void PktConnect::clientId(const char *str)
{
	this->clientId((const uint8_t *)str, strlen(str));
}

void PktConnect::will(const uint8_t *topic, uint16_t topic_size,
		      const uint8_t *msg, uint16_t msg_size)
{
	if (topic == nullptr || topic_size == 0 || msg == nullptr || msg_size == 0) {
		throw std::invalid_argument("Invalid Will Topic or Msg length");
	}

	this->_willTopic.assign(topic, topic + topic_size);
	this->_willMsg.assign(msg, msg + msg_size);
}

void PktConnect::will(const char *topic, const char *msg)
{
	this->will((const uint8_t *)topic, strlen(topic),
		   (const uint8_t *)msg, strlen(msg));
}

void PktConnect::userName(const uint8_t *data, uint16_t size)
{
	this->_userName.assign(data, data + size);
}

void PktConnect::userName(const char *str)
{
	this->userName((const uint8_t *)str, strlen(str));
}

void PktConnect::password(const uint8_t *data, uint16_t size)
{
	this->_password.assign(data, data + size);
}

void PktConnect::password(const char *str)
{
	this->password((const uint8_t *)str, strlen(str));
}

void PktConnect::keepAlive(uint16_t keepAlive)
{
	this->_keepAlive = keepAlive;
}

void PktConnect::willRetain(bool willRetain)
{
	this->_willRetain = (willRetain ? 1 : 0);
}

void PktConnect::willQoS(enum PktQoS qos)
{
	this->_willQoS = (uint8_t)qos;
}

void PktConnect::cleanStart(bool cleanStart)
{
	this->_cleanStart = (cleanStart ? 1 : 0);
}

}

