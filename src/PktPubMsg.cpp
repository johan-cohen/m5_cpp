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

#include "PktPubMsg.hpp"

namespace m5 {

PktPubMsg::PktPubMsg(enum PktType type, uint8_t reserved) :
	Packet(type, reserved)
{
}

PktPubMsg::PktPubMsg(enum PktType type, uint8_t reserved, AppBuf &buf) :
	Packet(type, reserved)
{
	this->readFrom(buf);
}

void PktPubMsg::packetId(uint16_t id)
{
	if (id == 0) {
		throw std::invalid_argument("Invalid packet Id");
	}

	this->_packetId = id;
}

void PktPubMsg::reasonCode(enum ReasonCode rc)
{
	this->_reasonCode = (uint8_t)rc;
}

void PktPubMsg::reasonString(const uint8_t *data, uint16_t size)
{
	properties.reasonString(data, size);
}

void PktPubMsg::reasonString(const char *str)
{
	properties.reasonString(str);
}

const ByteArray &PktPubMsg::reasonString(void) const
{
	return properties.reasonString();
}

void PktPubMsg::userProperty(const uint8_t *key, uint16_t keySize,
			     const uint8_t *value, uint16_t valueSize)
{
	properties.userProperty(key, keySize, value, valueSize);
}

void PktPubMsg::userProperty(const char *key, const char *val)
{
	properties.userProperty(key, val);
}

const UserProperty &PktPubMsg::userProperty(void) const
{
	return properties.userProperty();
}

enum StatusCode PktPubMsg::writeVariableHeader(AppBuf &buf)
{
	buf.writeNum16(this->packetId());
	buf.writeNum8((uint8_t)this->reasonCode());

	if (properties.write(buf) == 0) {
		return StatusCode::PROPERTY_WRITE_ERROR;
	}

	return StatusCode::SUCCESS;
}

enum StatusCode PktPubMsg::writePayload(AppBuf &buf)
{
	(void)buf;

	return StatusCode::SUCCESS;
}

uint32_t PktPubMsg::writeTo(AppBuf &buf)
{
	if (this->packetId() == 0) {
		throw std::invalid_argument("Invalid packet Id");
	}

	Packet::variableHeaderSize = 2 + 1;
	Packet::hasProperties = true;

	return Packet::writeTo(buf);
}

enum StatusCode PktPubMsg::readVariableHeader(AppBuf &buf)
{
	this->packetId(buf.readNum16());
	if (this->packetId() == 0) {
		return StatusCode::INVALID_PACKET_ID;
	}

	this->reasonCode((enum ReasonCode)buf.readNum8());

	properties.read(buf);

	return StatusCode::SUCCESS;
}

enum StatusCode PktPubMsg::readPayload(AppBuf &buf)
{
	(void)buf;

	return StatusCode::SUCCESS;
}

uint32_t PktPubMsg::readFrom(AppBuf &buf)
{
	Packet::minRemLen = packetIdSize + reasonCodeSize + propertyMinSize;

	return Packet::readFrom(buf);
}

}

