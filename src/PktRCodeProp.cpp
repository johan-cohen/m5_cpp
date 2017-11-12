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

PktRCodeProp::PktRCodeProp(PktType type, AppBuf &buf) :
	_packetType(type), properties(type)
{
	this->readFrom(buf);
}

void PktRCodeProp::reasonCode(ReasonCode rc)
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

uint32_t PktRCodeProp::writeTo(AppBuf &buf)
{
	uint32_t fullPktSize;
	uint32_t propWSWS;
	uint32_t propWS;
	uint32_t remLenWS;
	uint32_t remLen;

	propWS = properties.wireSize();
	propWSWS = VBIWireSize(propWS);

	remLen = 1 + propWSWS + propWS;
	remLenWS = VBIWireSize(remLen);

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	buf.writeNum8(m5::firstByte(this->_packetType, 0));
	buf.writeVBI(remLen);
	buf.writeNum8(this->_reasonCode);
	properties.write(buf);

	return fullPktSize;
}

uint32_t PktRCodeProp::readFrom(AppBuf &buf)
{
	std::size_t alreadyTraversed = buf.traversed();
	uint8_t remLenWS;
	uint32_t remLen;

	if (buf.bytesToRead() < 3) {
		throw std::out_of_range("No enough space in input buffer");
	}

	if (buf.readNum8() != m5::firstByte(this->_packetType)) {
		throw std::invalid_argument("Msg not found in buf");
	}

	buf.readVBI(remLen, remLenWS);
	if (remLen < 1) {
		throw std::out_of_range("No enough space in input buffer");
	}

	this->_reasonCode = buf.readNum8();
	properties.read(buf);

	uint32_t fullPktSize = 1 + remLenWS + remLen;
	if (buf.traversed() - alreadyTraversed != fullPktSize) {
		throw std::invalid_argument("Corrupted input buffer");
	}

	return fullPktSize;
}

}

