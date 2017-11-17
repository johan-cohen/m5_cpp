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

#include "PktSubscribe.hpp"

#include <cstring>
#include <cerrno>

namespace m5 {

TopicOptions::TopicOptions(const uint8_t *topic, uint16_t size, uint8_t options) :
	                   topic(topic, topic + size),
			   options(options)
{
}

TopicOptions::TopicOptions(const char *topic, uint8_t options) :
	                   topic(topic, topic + strlen(topic)),
			   options(options)
{
}

PktSubscribe::PktSubscribe() : properties(PktType::SUBSCRIBE)
{
}

PktSubscribe::PktSubscribe(AppBuf &buf) : properties(PktType::SUBSCRIBE)
{
	this->readFrom(buf);
}

PktSubscribe::~PktSubscribe()
{
	auto it = _topics.begin();

	while (it != _topics.end()) {
		delete (*it);

		it++;
	}
}

void PktSubscribe::append(const uint8_t *topic, uint16_t size, uint8_t options)
{
	TopicOptions *item;

	item = new TopicOptions(topic, size, options);
	payloadWS += stringLenSize + item->topic.size() + 1;

	this->_topics.push_back(item);
}


void PktSubscribe::append(const char *topic, uint8_t options)
{
	append((const uint8_t *)topic, strlen(topic), options);
}

void PktSubscribe::subscriptionIdentifier(uint32_t v)
{
	properties.subscriptionIdentifier(v);
}

uint32_t PktSubscribe::subscriptionIdentifier(void) const
{
	return properties.subscriptionIdentifier();
}

void PktSubscribe::writePayload(AppBuf &buf)
{
	auto it = _topics.begin();

	while (it != _topics.end()) {
		auto item = *it;

		buf.writeBinary(item->topic);
		buf.writeNum8(item->options);

		it++;
	}
}

uint32_t PktSubscribe::writeTo(AppBuf &buf)
{
	const auto initialLength = buf.length();
	uint32_t fullPktSize;
	uint32_t propWSWS;
	uint32_t propWS;
	uint32_t remLenWS;
	uint32_t remLen;

	if (payloadWS == 0) {
		throw std::invalid_argument("No topics in SUBSCRIBE msg");
	}

	if (packetId() == 0) {
		throw std::invalid_argument("Invalid Packet ID");
	}

	propWS = properties.wireSize();
	propWSWS = VBIWireSize(propWS);
	if (propWSWS == 0) {
		return 0;
	}

	remLen = 2 + propWSWS + propWS + payloadWS;
	remLenWS = VBIWireSize(remLen);
	if (remLenWS == 0) {
		return 0;
	}

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	buf.writeNum8(m5::firstByte(PktType::SUBSCRIBE, 0x02));
	buf.writeVBI(remLen);
	buf.writeNum16(packetId());
	if (properties.write(buf) != propWSWS + propWS) {
		return buf.length() - initialLength;
	}
	writePayload(buf);

	return fullPktSize;
}

void PktSubscribe::readPayload(AppBuf &buf)
{
	do {
		if (buf.bytesToRead() < 2) {
			throw std::out_of_range("No enough space in input buffer");
		}

		auto len = buf.readNum16();

		if (buf.bytesToRead() < (uint16_t)(len + 1)) {
			throw std::out_of_range("No enough space in input buffer");
		}

		auto ptr = buf.ptrRead();
		buf.readSkip(len);
		auto options = buf.readNum8();

		this->append(ptr, len, options);
	} while (buf.bytesToRead() > 1);
}

uint32_t PktSubscribe::readFrom(AppBuf &buf)
{
	std::size_t alreadyTraversed = buf.traversed();
	uint8_t remLenWS;
	uint32_t remLen;
	int rc;

	if (buf.bytesToRead() < 8) {
		throw std::out_of_range("No enough space in input buffer");
	}

	if (buf.readNum8() != m5::firstByte(PktType::SUBSCRIBE, 0x02)) {
		throw std::invalid_argument("Invalid fixed header");
	}

	rc = buf.readVBI(remLen, remLenWS);
	if (rc != EXIT_SUCCESS) {
		return remLenWS;
	}

	if (remLen < 6) {
		throw std::out_of_range("No enough space in input buffer");
	}

	this->packetId(buf.readNum16());
	properties.read(buf);

	this->payloadWS = buf.bytesToRead();
	readPayload(buf);
	this->payloadWS -= buf.bytesToRead();

	uint32_t fullPktSize = 1 + remLenWS + remLen;
	if (buf.traversed() - alreadyTraversed != fullPktSize) {
		throw std::invalid_argument("Corrupted input buffer");
	}

	return fullPktSize;
}

}

