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

#include "PktUnsubscribe.hpp"

#include <cstring>

namespace m5 {

PktUnsubscribe::PktUnsubscribe(AppBuf &buf)
{
	this->readFrom(buf);
}

PktUnsubscribe::~PktUnsubscribe()
{
	auto it = _topics.begin();
	while (it != _topics.end()) {
		delete *it;
		it++;
	}
}

void PktUnsubscribe::append(const char *str)
{
	ByteArray *item;

	item = new ByteArray(str, str + strlen(str));
	this->_topics.push_back(item);

	payloadSize += stringLenSize + item->size();
}

uint32_t PktUnsubscribe::writeTo(AppBuf &buf)
{
	uint32_t fullPktSize;
	uint32_t remLenWS;
	uint32_t remLen;

	if (this->packetId() == 0) {
		throw std::invalid_argument("Invalid packet Id");
	}

	remLen = 2 + this->payloadSize;
	remLenWS = VBIWireSize(remLen);

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	buf.writeNum8(((uint8_t)PktType::UNSUBSCRIBE << 4) | 0x02);
	buf.writeVBI(remLen);
	buf.writeNum16(this->packetId());

	for (auto it = _topics.begin(); it != _topics.end(); it++) {
		ByteArray *item = *it;
		buf.writeBinary(*item);
	}

	return fullPktSize;
}

uint32_t PktUnsubscribe::readFrom(AppBuf &buf)
{
	std::size_t alreadyTraversed = buf.traversed();
	uint32_t remLen;
	uint8_t remLenWS;
	uint8_t first;

	if (buf.bytesToRead() < 7) {
		throw std::invalid_argument("Invalid input buffer");
	}

	first = buf.readNum8();
	if (first != (((uint8_t)PktType::UNSUBSCRIBE << 4) | 0x02)) {
		throw std::invalid_argument("SUBACK: Invalid packet type");
	}

	buf.readVBI(remLen, remLenWS);
	if (remLen > buf.bytesToRead()) {
		throw std::out_of_range("No enough space in input buffer");
	}

	this->packetId(buf.readNum16());
	if (this->packetId() == 0) {
		throw std::invalid_argument("Invalid packet Id");
	}

	uint32_t minRemLen = 2;
	while (minRemLen < remLen) {
		ByteArray *item;

		item =  buf.readBinary();
		this->_topics.push_back(item);
		minRemLen += 2 + item->size();
	}

	uint32_t fullPktSize = 1 + remLenWS + remLen;
	if (buf.traversed() - alreadyTraversed != fullPktSize) {
		throw std::invalid_argument("Corrupted input buffer");
	}

	return fullPktSize;
}

}

