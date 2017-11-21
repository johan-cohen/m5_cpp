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

#include "PktConnAck.hpp"

namespace m5 {

PktConnAck::PktConnAck(bool sessionPresent, ReasonCode reasonCode) :
		       Packet(PktType::CONNACK, 0x00)
{
	this->_sessionPresent = sessionPresent;
	this->_reasonCode = (uint8_t)reasonCode;
}

PktConnAck::PktConnAck(AppBuf &buf) : Packet(PktType::CONNACK, 0x00)
{
	this->readFrom(buf);
}

enum StatusCode PktConnAck::assignedClientIdentifier(const uint8_t *data,
						     uint16_t size)
{
	return properties.assignedClientIdentifier(data, size);
}

enum StatusCode PktConnAck::assignedClientIdentifier(const char *str)
{
	return properties.assignedClientIdentifier(str);
}

const ByteArray &PktConnAck::assignedClientIdentifier(void) const
{
	return properties.assignedClientIdentifier();
}

enum StatusCode PktConnAck::serverKeepAlive(uint16_t v)
{
	return properties.serverKeepAlive(v);
}

uint16_t PktConnAck::serverKeepAlive(void) const
{
	return properties.serverKeepAlive();
}

enum StatusCode PktConnAck::authenticationMethod(const uint8_t *data, uint16_t size)
{
	return properties.authenticationMethod(data, size);
}

enum StatusCode PktConnAck::authenticationMethod(const char *str)
{
	return properties.authenticationMethod(str);
}

const ByteArray &PktConnAck::authenticationMethod(void) const
{
	return properties.authenticationMethod();
}

enum StatusCode PktConnAck::authenticationData(const uint8_t *data, uint16_t size)
{
	return properties.authenticationData(data, size);
}

const ByteArray &PktConnAck::authenticationData(void) const
{
	return properties.authenticationData();
}

enum StatusCode PktConnAck::responseInformation(const uint8_t *data, uint16_t size)
{
	return properties.responseInformation(data, size);
}

enum StatusCode PktConnAck::responseInformation(const char *str)
{
	return properties.responseInformation(str);
}

const ByteArray &PktConnAck::responseInformation(void) const
{
	return properties.responseInformation();
}

enum StatusCode PktConnAck::serverReference(const uint8_t *data, uint16_t size)
{
	return properties.serverReference(data, size);
}

enum StatusCode PktConnAck::serverReference(const char *str)
{
	return properties.serverReference(str);
}

const ByteArray &PktConnAck::serverReference(void) const
{
	return properties.serverReference();
}

enum StatusCode PktConnAck::reasonString(const uint8_t *data, uint16_t size)
{
	return properties.reasonString(data, size);
}

enum StatusCode PktConnAck::reasonString(const char *str)
{
	return properties.reasonString(str);
}

const ByteArray &PktConnAck::reasonString(void) const
{
	return properties.reasonString();
}

enum StatusCode PktConnAck::receiveMaximum(uint16_t v)
{
	return properties.receiveMaximum(v);
}

uint16_t PktConnAck::receiveMaximum(void) const
{
	return properties.receiveMaximum();
}

enum StatusCode PktConnAck::topicAliasMaximum(uint16_t v)
{
	return properties.topicAliasMaximum(v);
}

uint16_t PktConnAck::topicAliasMaximum(void) const
{
	return properties.topicAliasMaximum();
}

enum StatusCode PktConnAck::maximumQoS(enum PktQoS qos)
{
	return properties.maximumQoS(qos);
}

enum PktQoS PktConnAck::maximumQoS(void) const
{
	return properties.maximumQoS();
}

enum StatusCode PktConnAck::retainAvailable(bool v)
{
	return properties.retainAvailable(v);
}

bool PktConnAck::retainAvailable(void) const
{
	return properties.retainAvailable();
}

enum StatusCode PktConnAck::userProperty(const uint8_t *key, uint16_t keySize,
					 const uint8_t *value, uint16_t valueSize)
{
	return properties.userProperty(key, keySize, value, valueSize);
}

enum StatusCode PktConnAck::userProperty(const char *key, const char *val)
{
	return properties.userProperty(key, val);
}

const UserProperty &PktConnAck::userProperty(void) const
{
	return properties.userProperty();
}

enum StatusCode PktConnAck::maximumPacketSize(uint32_t v)
{
	return properties.maximumPacketSize(v);
}

uint32_t PktConnAck::maximumPacketSize(void) const
{
	return properties.maximumPacketSize();
}

enum StatusCode PktConnAck::wildcardSubscriptionAvailable(bool v)
{
	return properties.wildcardSubscriptionAvailable(v);
}

bool PktConnAck::wildcardSubscriptionAvailable(void) const
{
	return properties.wildcardSubscriptionAvailable();
}

enum StatusCode PktConnAck::subscriptionIdentifierAvailable(bool v)
{
	return properties.subscriptionIdentifierAvailable(v);
}

bool PktConnAck::subscriptionIdentifierAvailable(void) const
{
	return properties.subscriptionIdentifierAvailable();
}

enum StatusCode PktConnAck::sharedSubscriptionAvailable(bool v)
{
	return properties.sharedSubscriptionAvailable(v);
}

bool PktConnAck::sharedSubscriptionAvailable(void) const
{
	return properties.sharedSubscriptionAvailable();
}

enum StatusCode PktConnAck::writeVariableHeader(AppBuf &buf)
{
	buf.writeNum8(this->_sessionPresent ? 0x01 : 0x00);
	buf.writeNum8(this->_reasonCode);

	return properties.write(buf);
}

enum StatusCode PktConnAck::writePayload(AppBuf &buf)
{
	(void)buf;

	return StatusCode::SUCCESS;
}


uint32_t PktConnAck::writeTo(AppBuf &buf)
{
	Packet::variableHeaderSize = 1 + 1;
	Packet::hasProperties = true;

	return Packet::writeTo(buf);
}

enum StatusCode PktConnAck::readVariableHeader(AppBuf &buf)
{
	auto number = buf.readNum8();
	if (number & 0xFD) {
		return StatusCode::RESERVED_MUST_BE_ZERO;
	}
	this->_sessionPresent = number ? true : false;
	this->_reasonCode = buf.readNum8();

	return properties.read(buf);
}

enum StatusCode PktConnAck::readPayload(AppBuf &buf)
{
	(void)buf;

	return StatusCode::SUCCESS;
}

uint32_t PktConnAck::readFrom(AppBuf &buf)
{
	Packet::minRemLen = flagsSize + reasonCodeSize + propertyMinSize;

	return Packet::readFrom(buf);
}

}

