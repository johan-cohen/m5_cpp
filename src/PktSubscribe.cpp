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

namespace m5 {

TopicOptions::TopicOptions(const char *topic, uint8_t options) :
			   topic(topic, topic + strlen(topic)),
			   options(options)
{

}

PktSubscribe::PktSubscribe() : properties(PktType::SUBSCRIBE)
{
}

PktSubscribe::~PktSubscribe()
{
	auto it = _topics.begin();

	while (it != _topics.end()) {
		delete (*it);

		it++;
	}
}

void PktSubscribe::append(const char *topic, uint8_t options)
{
	TopicOptions *item;

	item = new TopicOptions(topic, options);
	payloadWS += stringLenSize + item->topic.size() + 1;

	this->_topics.push_back(item);
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

	remLen = 2 + propWSWS + propWS + payloadWS;
	remLenWS = VBIWireSize(remLen);

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	buf.writeNum8((uint8_t)PktType::SUBSCRIBE << 4 | 0x02);
	buf.writeVBI(remLen);
	buf.writeNum16(packetId());
	properties.write(buf);
	writePayload(buf);

	return fullPktSize;
}

uint32_t PktSubscribe::readFrom(AppBuf &buf)
{
	(void)buf;

	return 0;

}

}

