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

#include "PktPublish.hpp"

#include <cstring>

namespace m5 {

PktPublish::PktPublish() : properties(PktType::PUBLISH)
{

}

PktPublish::~PktPublish()
{

}

void PktPublish::topic(const uint8_t *data, uint16_t size)
{
	if (data == nullptr || size < 1) {
		throw std::invalid_argument("Invalid input buffer");
	}

	this->_topic.assign(data, data + size);
}

void PktPublish::topic(const char *str)
{
	topic((const uint8_t *)str, strlen(str));
}

void PktPublish::payload(const uint8_t *data, uint16_t size)
{
	if (data == nullptr || size < 1) {
		throw std::invalid_argument("Invalid input buffer");
	}

	this->_payload.assign(data, data + size);
}

uint32_t PktPublish::writeTo(AppBuf &buf)
{
	uint8_t firstByte = 0;
	uint32_t fullPktSize;
	uint32_t propWSWS;
	uint32_t propWS;
	uint32_t remLenWS;
	uint32_t remLen;

	if (this->QoS() != PktQoS::QoS0 && this->packetId() == 0) {
		throw std::out_of_range("Invalid packet Id for this QoS level");
	}

	if (this->topic().size() == 0) {
		throw std::out_of_range("Invalid topic name");
	}

	propWS = properties.wireSize();
	propWSWS = VBIWireSize(propWS);

	remLen = stringLenSize + topic().size() +
		 propWSWS + propWS + payload().size();
	if (this->QoS() != PktQoS::QoS0) {
		remLen += 2;
	}
	remLenWS = VBIWireSize(remLen);

	fullPktSize = 1 + remLenWS + remLen;
	if (buf.bytesToWrite() < fullPktSize) {
		throw std::out_of_range("No enough space in buffer");
	}

	firstByte += ((int)PktType::PUBLISH) << 4;
	firstByte += this->dup() ? 1 << 3 : 0;
	firstByte += ((int)this->QoS() & 0x03) << 1;
	firstByte += this->retain() ? 1 : 0;

	buf.writeNum8(firstByte);
	buf.writeVBI(remLen);
	if (this->QoS() != PktQoS::QoS0) {
		buf.writeBinary(this->topic());
	}
	buf.writeNum16(this->packetId());
	properties.write(buf);
	buf.write(this->payload().data(), this->payload().size());

	return fullPktSize;
}

uint32_t PktPublish::readFrom(AppBuf &buf)
{
	(void)buf;

	return 0;
}

}

