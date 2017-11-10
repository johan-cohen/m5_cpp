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

#include"PktSubAckMsg.hpp"

namespace m5 {

PktSubAckMsg::PktSubAckMsg(PktType type) : _packetType((uint8_t)type), properties(type)
{
}

PktSubAckMsg::PktSubAckMsg(AppBuf &buf)
{
	this->readFrom(buf);
}


uint32_t PktSubAckMsg::writeTo(AppBuf &buf)
{
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

	remLen = 2 + propWSWS + propWS + _reasonCodes.size();
	remLenWS = VBIWireSize(remLen);

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	buf.writeNum8((uint8_t)this->getId() << 4);
	buf.writeVBI(remLen);
	buf.writeNum16(this->packetId());
	properties.write(buf);

	for (auto it = _reasonCodes.begin(); it != _reasonCodes.end(); it++) {
		buf.writeNum8(*it);
	}

	return fullPktSize;
}

uint32_t PktSubAckMsg::readFrom(AppBuf &buf)
{
	std::size_t alreadyTraversed = buf.traversed();
	uint32_t remLen;
	uint8_t remLenWS;
	uint8_t first;

	if (buf.bytesToRead() < 6) {
		throw std::invalid_argument("Invalid input buffer");
	}

	first = buf.readNum8();
	if (first != ((uint8_t)this->getId() << 4)) {
		throw std::invalid_argument("Invalid packet type");
	}

	buf.readVBI(remLen, remLenWS);
	if (remLen > buf.bytesToRead()) {
		throw std::out_of_range("No enough space in input buffer");
	}

	this->packetId(buf.readNum16());
	if (this->packetId() == 0) {
		throw std::invalid_argument("Invalid packet Id");
	}

	properties.read(buf);

	auto propertiesFullSize = VBIWireSize(properties.wireSize()) + properties.wireSize();
	auto minRemLen = 2 + propertiesFullSize;
	if (minRemLen >= remLen) {
		throw std::out_of_range("Corrupted input buffer");
	}

	while (minRemLen < remLen) {
		/* no need to check for bytesToRead here */
		this->append((ReasonCode)buf.readNum8());
		minRemLen++;
	}

	uint32_t fullPktSize = 1 + remLenWS + remLen;
	if (buf.traversed() - alreadyTraversed != fullPktSize) {
		throw std::invalid_argument("Corrupted input buffer");
	}

	return fullPktSize;
}

}

