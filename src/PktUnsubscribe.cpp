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
#include <cerrno>

namespace m5 {

PktUnsubscribe::PktUnsubscribe() : Packet(PktType::UNSUBSCRIBE, 0x02)
{
}

PktUnsubscribe::PktUnsubscribe(AppBuf &buf) : Packet(PktType::UNSUBSCRIBE, 0x02)
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

enum StatusCode PktUnsubscribe::writeVariableHeader(AppBuf &buf)
{
	buf.writeNum16(this->packetId());

	return StatusCode::SUCCESS;
}

enum StatusCode PktUnsubscribe::writePayload(AppBuf &buf)
{
	for (auto it = _topics.begin(); it != _topics.end(); it++) {
		ByteArray *item = *it;
		buf.writeBinary(*item);
	}

	return StatusCode::SUCCESS;
}

uint32_t PktUnsubscribe::writeTo(AppBuf &buf)
{
	if (this->packetId() == 0) {
		throw std::invalid_argument("Invalid packet Id");
	}

	Packet::variableHeaderSize = 2;
	Packet::hasProperties = false;

	return Packet::writeTo(buf);
}

enum StatusCode PktUnsubscribe::fixedHeaderFlags(uint8_t flags)
{
	if ((flags & 0x02) != 0x02) {
		return StatusCode::INVALID_FIXED_HEADER;
	}

	return StatusCode::SUCCESS;
}

enum StatusCode PktUnsubscribe::readVariableHeader(AppBuf &buf)
{
	this->packetId(buf.readNum16());
	if (this->packetId() == 0) {
		return StatusCode::INVALID_PACKET_ID;
	}

	return StatusCode::SUCCESS;
}

enum StatusCode PktUnsubscribe::readPayload(AppBuf &buf)
{
	uint32_t minRemLen = 2;

	while (minRemLen < remainingLength) {
		ByteArray *topic;

		topic =  buf.readBinary();
		if (topic->size() == 0) {
			return StatusCode::INVALID_TOPIC_NAME;
		}

		this->_topics.push_back(topic);
		minRemLen += 2 + topic->size();
	}

	return StatusCode::SUCCESS;
}

uint32_t PktUnsubscribe::readFrom(AppBuf &buf)
{
	Packet::minBufferSize = 7;
	Packet::minRemLen = 5;

	return Packet::readFrom(buf);
}

}

