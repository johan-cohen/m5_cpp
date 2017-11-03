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

PktPubMsg::PktPubMsg(PktType type, uint8_t reserved)
{
	this->_type = type;
	this->_reserved = reserved;
}

void PktPubMsg::packetId(uint16_t id)
{
	if (id == 0) {
		throw std::invalid_argument("Invalid packet Id");
	}

	this->_packetId = id;
}

void PktPubMsg::reasonCode(ReasonCode rc)
{
	this->_reasonCode = rc;
}

uint32_t PktPubMsg::writeTo(AppBuf &buf)
{
	uint8_t firstByte = 0;
	uint32_t fullPktSize;
	uint32_t propWSWS;
	uint32_t propWS;
	uint32_t remLenWS;
	uint32_t remLen;

	if (this->packetId() == 0) {
		throw std::invalid_argument("Invalid packet Id");
	}

	propWS = properties.wireSize();
	propWSWS = VBIWireSize(propWS);

	remLen = 2 + 1 + propWSWS + propWS;
	remLenWS = VBIWireSize(remLen);

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	firstByte = ((uint8_t)this->_type << 4) + (this->_reserved & 0x0F);
	buf.writeNum8(firstByte);
	buf.writeVBI(remLen);
	buf.writeNum16(this->packetId());
	buf.writeNum8((uint8_t)this->reasonCode());
	properties.write(buf);

	return fullPktSize;
}

uint32_t PktPubMsg::readFrom(AppBuf &buf)
{
	(void)buf;

	return 0;
}

}

