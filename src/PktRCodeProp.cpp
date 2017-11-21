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

#include "PktRCodeProp.hpp"

namespace m5 {

PktRCodeProp::PktRCodeProp(enum PktType type) : Packet(type, 0x00)
{
}

PktRCodeProp::PktRCodeProp(enum PktType type, AppBuf &buf) : Packet(type, 0x00)
{
	this->readFrom(buf);
}

void PktRCodeProp::reasonCode(enum ReasonCode rc)
{
	this->_reasonCode = (uint8_t)rc;
}


void PktRCodeProp::reasonString(const uint8_t *data, uint16_t size)
{
	properties.reasonString(data, size);
}

void PktRCodeProp::reasonString(const char *str)
{
	properties.reasonString(str);
}

const ByteArray &PktRCodeProp::reasonString(void) const
{
	return properties.reasonString();
}

void PktRCodeProp::userProperty(const uint8_t *key, uint16_t keySize,
				const uint8_t *value, uint16_t valueSize)
{
	properties.userProperty(key, keySize, value, valueSize);
}

void PktRCodeProp::userProperty(const char *key, const char *val)
{
	properties.userProperty(key, val);
}

const UserProperty &PktRCodeProp::userProperty(void) const
{
	return properties.userProperty();
}

enum StatusCode PktRCodeProp::writeVariableHeader(AppBuf &buf)
{
	buf.writeNum8(this->_reasonCode);

	return properties.write(buf);
}

enum StatusCode PktRCodeProp::writePayload(AppBuf &buf)
{
	(void)buf;

	return StatusCode::SUCCESS;
}

uint32_t PktRCodeProp::writeTo(AppBuf &buf)
{
	Packet::variableHeaderSize = 1;
	Packet::hasProperties = true;

	return Packet::writeTo(buf);
}

enum StatusCode PktRCodeProp::readVariableHeader(AppBuf &buf)
{
	if (remainingLength > 0) {
		reasonCode((enum ReasonCode)buf.readNum8());
		if (remainingLength > 1) {
			return properties.read(buf);
		}
	} else {
		reasonCode((enum ReasonCode)0x00);
	}

	return StatusCode::SUCCESS;
}

enum StatusCode PktRCodeProp::readPayload(AppBuf &buf)
{
	(void)buf;

	return StatusCode::SUCCESS;
}

uint32_t PktRCodeProp::readFrom(AppBuf &buf)
{
	Packet::minRemLen = 0;

	return Packet::readFrom(buf);
}

}

