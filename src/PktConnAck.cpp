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
	properties(PktType::CONNACK)
{
	this->_sessionPresent = sessionPresent;
	this->_reasonCode = (uint8_t)reasonCode;
}

uint32_t PktConnAck::writeTo(AppBuf &buf)
{
	uint32_t fullPktSize;
	uint32_t propWSWS;
	uint32_t propWS;
	uint32_t remLenWS;
	uint32_t remLen;

	propWS = properties.wireSize();
	propWSWS = VBIWireSize(propWS);

	remLen = 2 + propWSWS + propWS;
	remLenWS = VBIWireSize(remLen);

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	buf.writeNum8(((uint8_t)PktType::CONNACK) << 4);
	buf.writeVBI(remLen);
	buf.writeNum8(this->_sessionPresent ? 0x01 : 0x00);
	buf.writeNum8(this->_reasonCode);
	properties.write(buf);

	return fullPktSize;
}

uint32_t PktConnAck::readFrom(AppBuf &buf)
{
	(void)buf;

	return 0;
}

}