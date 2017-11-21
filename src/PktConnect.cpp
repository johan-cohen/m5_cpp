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

#include <cstring>

namespace  m5 {

static const uint32_t clientIdMinLen = 1;
static const uint32_t clientIdMaxLen = 23;

static const uint8_t protocolVersion5 = 0x05;

static const char protocolStr[] = "MQTT";
static const uint8_t protocolNameStr[] = {0, 4, 'M', 'Q', 'T', 'T'};
static const uint32_t protocolNameWireSize = sizeof(protocolNameStr);

/* Protocol Name (2 + 4), Protocol Level (1), Conn Flags (1), Keep Alive (2)
 * No properties are considered here.
 */
static const uint32_t varHeaderSize = sizeof(protocolNameStr) + 1 + flagsSize + 2;

PktConnect::PktConnect() : Packet(PktType::CONNECT, 0x00)
{
}

PktConnect::PktConnect(const uint8_t *clientId, uint16_t len, bool cleanStart) :
		       Packet(PktType::CONNECT, 0x0)
{
	init(clientId, len, cleanStart);
}

PktConnect::PktConnect(const char *clientId, bool cleanStart) :
		       Packet(PktType::CONNECT, 0x0)
{
	init((const uint8_t *)clientId, strlen(clientId), cleanStart);
}

PktConnect::PktConnect(AppBuf &buf) : Packet(PktType::CONNECT, 0x0)
{
	this->readFrom(buf);
}

PktConnect::~PktConnect()
{
}

void PktConnect::init(const uint8_t *clientId, uint16_t len, bool cleanStart)
{
	this->clientId(clientId, len);
	this->cleanStart(cleanStart);
}

uint8_t PktConnect::packConnectFlags(void)
{
	uint8_t flags;

	flags = (cleanStart() << 0x01);
	flags += (willMsg().size() > 0 ? (0x01 << 2) : 0);
	flags += (((uint8_t)willQoS() & 0x03) << 3);
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

enum PktQoS PktConnect::flagWillQoS(uint8_t flags)
{
	return (enum PktQoS)((flags & (3 << 3)) >> 3);
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

bool PktConnect::validClientIdSize(uint16_t size)
{
	return (size >= clientIdMinLen && size <= clientIdMaxLen);
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

enum StatusCode PktConnect::writeVariableHeader(AppBuf &buf)
{
	buf.writeString(protocolStr);
	buf.writeNum8(protocolVersion5);
	buf.writeNum8(packConnectFlags());
	buf.writeNum16(keepAlive());

	return properties.write(buf);
}

enum StatusCode PktConnect::writePayload(AppBuf &buf)
{
	buf.writeBinary(clientId().data(), clientId().size());

	if (willMsg().size() > 0) {
		buf.writeBinary(willTopic().data(), willTopic().size());
		buf.writeBinary(willMsg().data(), willMsg().size());
	}

	if (userName().size() > 0) {
		buf.writeBinary(userName().data(), userName().size());
	}

	if (password().size() > 0) {
		buf.writeBinary(password().data(), password().size());
	}

	return StatusCode::SUCCESS;
}

uint32_t PktConnect::writeTo(AppBuf &buf)
{
	Packet::variableHeaderSize = varHeaderSize;
	Packet::payloadSize = payloadWireSize();
	Packet::hasProperties = true;

	return Packet::writeTo(buf);
}

enum StatusCode PktConnect::readVariableHeader(AppBuf &buf)
{
	if (memcmp(buf.ptrRead(), protocolNameStr, protocolNameWireSize) != 0) {
		return StatusCode::INVALID_PROTOCOL_NAME;
	}
	buf.readSkip(protocolNameWireSize);

	if (buf.readNum8() != protocolVersion5) {
		return StatusCode::INVALID_PROTOCOL_VERSION;
	}

	connectFlags = buf.readNum8();
	this->cleanStart(flagCleanStart(connectFlags));
	this->willRetain(flagWillRetain(connectFlags));
	this->willQoS(flagWillQoS(connectFlags));

	keepAlive(buf.readNum16());

	return properties.read(buf);
}

enum StatusCode PktConnect::readPayload(AppBuf &buf)
{
	enum StatusCode rc;

	rc = buf.readBinary(_clientId);
	if (rc != StatusCode::SUCCESS || !validClientIdSize(clientId().size())) {
		return StatusCode::INVALID_CONNECT_PAYLOAD;
	}

	if (flagWillMsg(connectFlags) == true) {
		rc = buf.readBinary(_willTopic);
		if (rc != StatusCode::SUCCESS) {
			return StatusCode::INVALID_CONNECT_PAYLOAD;
		}

		rc = buf.readBinary(_willMsg);
		if (rc != StatusCode::SUCCESS) {
			return StatusCode::INVALID_CONNECT_PAYLOAD;
		}
	}

	if (flagUserName(connectFlags) == true) {
		rc = buf.readBinary(_userName);
		if (rc != StatusCode::SUCCESS) {
			return StatusCode::INVALID_CONNECT_PAYLOAD;
		}
	}

	if (flagPassword(connectFlags) == true) {
		rc = buf.readBinary(_password);
		if (rc != StatusCode::SUCCESS) {
			return StatusCode::INVALID_CONNECT_PAYLOAD;
		}
	}

	return StatusCode::SUCCESS;
}

uint32_t PktConnect::readFrom(AppBuf &buf)
{
	Packet::minRemLen = varHeaderSize + propertyMinSize +
			    stringLenSize + clientIdMinLen;

	return Packet::readFrom(buf);
}

enum StatusCode PktConnect::clientId(const uint8_t *data, uint16_t size)
{
	if (!validClientIdSize(size)) {
		return StatusCode::INVALID_ARGUMENT;
	}

	this->_clientId.assign(data, data + size);
	return StatusCode::SUCCESS;
}

enum StatusCode PktConnect::clientId(const char *str)
{
	return this->clientId((const uint8_t *)str, strlen(str));
}

enum StatusCode PktConnect::will(const uint8_t *topic, uint16_t topic_size,
				 const uint8_t *msg, uint16_t msg_size)
{
	if (topic == nullptr || topic_size < topicNameMinSize ||
	    msg == nullptr || msg_size == 0) {
		return StatusCode::INVALID_ARGUMENT;
	}

	this->_willTopic.assign(topic, topic + topic_size);
	this->_willMsg.assign(msg, msg + msg_size);

	return StatusCode::SUCCESS;
}

enum StatusCode PktConnect::will(const char *topic, const char *msg)
{
	return this->will((const uint8_t *)topic, strlen(topic),
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

void PktConnect::cleanStart(bool cleanStart)
{
	this->_cleanStart = (cleanStart ? 1 : 0);
}

enum StatusCode PktConnect::sessionExpiryInterval(uint32_t v)
{
	return properties.sessionExpiryInterval(v);
}

uint32_t PktConnect::sessionExpiryInterval(void) const
{
	return properties.sessionExpiryInterval();
}

enum StatusCode PktConnect::authenticationMethod(const uint8_t *data, uint16_t size)
{
	return properties.authenticationMethod(data, size);
}

enum StatusCode PktConnect::authenticationMethod(const char *str)
{
	return properties.authenticationMethod(str);
}

const ByteArray &PktConnect::authenticationMethod(void) const
{
	return properties.authenticationMethod();
}

enum StatusCode PktConnect::authenticationData(const uint8_t *data, uint16_t size)
{
	return properties.authenticationData(data, size);
}

const ByteArray &PktConnect::authenticationData(void) const
{
	return properties.authenticationData();
}

enum StatusCode PktConnect::requestProblemInformation(bool v)
{
	return properties.requestProblemInformation(v);
}

bool PktConnect::requestProblemInformation(void) const
{
	return properties.requestProblemInformation();
}

enum StatusCode PktConnect::willDelayInterval(uint32_t v)
{
	return properties.willDelayInterval(v);
}

uint32_t PktConnect::willDelayInterval(void) const
{
	return properties.willDelayInterval();
}

enum StatusCode PktConnect::requestResponseInformation(bool v)
{
	return properties.requestResponseInformation(v);
}

bool PktConnect::requestResponseInformation(void) const
{
	return properties.requestResponseInformation();
}

enum StatusCode PktConnect::receiveMaximum(uint16_t v)
{
	return properties.receiveMaximum(v);
}

uint16_t PktConnect::receiveMaximum(void) const
{
	return properties.receiveMaximum();
}

enum StatusCode PktConnect::topicAliasMaximum(uint16_t v)
{
	return properties.topicAliasMaximum(v);
}

uint16_t PktConnect::topicAliasMaximum(void) const
{
	return properties.topicAliasMaximum();
}


enum StatusCode PktConnect::userProperty(const uint8_t *key, uint16_t keySize,
					 const uint8_t *value, uint16_t valueSize)
{
	return properties.userProperty(key, keySize, value, valueSize);
}

enum StatusCode PktConnect::userProperty(const char *key, const char *val)
{
	return properties.userProperty(key, val);
}

const UserProperty &PktConnect::userProperty(void) const
{
	return properties.userProperty();
}

enum StatusCode PktConnect::maximumPacketSize(uint32_t v)
{
	return properties.maximumPacketSize(v);
}

uint32_t PktConnect::maximumPacketSize(void) const
{
	return properties.maximumPacketSize();
}

}

