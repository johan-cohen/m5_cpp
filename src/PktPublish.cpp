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
#include <cerrno>

namespace m5 {

PktPublish::PktPublish() : Packet(PktType::PUBLISH, 0x00)
{
	hasProperties = true;
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
		return;
	}

	this->_payload.assign(data, data + size);
}

void PktPublish::payloadFormatIndicator(bool v)
{
	properties.payloadFormatIndicator(v);
}

bool PktPublish::payloadFormatIndicator(void) const
{
	return properties.payloadFormatIndicator();
}

void PktPublish::publicationExpiryInterval(uint32_t v)
{
	properties.publicationExpiryInterval(v);
}

uint32_t PktPublish::publicationExpiryInterval(void) const
{
	return properties.publicationExpiryInterval();
}

void PktPublish::contentType(const uint8_t *data, uint16_t size)
{
	properties.contentType(data, size);
}

void PktPublish::contentType(const char *str)
{
	properties.contentType(str);
}

const ByteArray &PktPublish::contentType(void) const
{
	return properties.contentType();
}

void PktPublish::responseTopic(const uint8_t *data, uint16_t size)
{
	properties.responseTopic(data, size);
}

void PktPublish::responseTopic(const char *str)
{
	properties.responseTopic(str);
}

const ByteArray &PktPublish::responseTopic(void) const
{
	return properties.responseTopic();
}

void PktPublish::subscriptionIdentifier(uint32_t v)
{
	properties.subscriptionIdentifier(v);
}

uint32_t PktPublish::subscriptionIdentifier(void) const
{
	return properties.subscriptionIdentifier();
}

void PktPublish::correlationData(const uint8_t *data, uint16_t size)
{
	properties.correlationData(data, size);
}

const ByteArray &PktPublish::correlationData(void) const
{
	return properties.correlationData();
}

void PktPublish::topicAlias(uint16_t v)
{
	properties.topicAlias(v);
}

uint16_t PktPublish::topicAlias(void) const
{
	return properties.topicAlias();
}

void PktPublish::userProperty(const uint8_t *key, uint16_t keySize,
			      const uint8_t *value, uint16_t valueSize)
{
	properties.userProperty(key, keySize, value, valueSize);
}

void PktPublish::userProperty(const char *key, const char *val)
{
	properties.userProperty(key, val);
}

const UserProperty &PktPublish::userProperty(void) const
{
	return properties.userProperty();
}

uint8_t PktPublish::headerFlags(void)
{
	uint8_t flags = 0;

	flags += this->dup() ? 1 << 3 : 0;
	flags += ((uint8_t)this->QoS() & 0x03) << 1;
	flags += this->retain() ? 1 : 0;

	return flags;
}

void PktPublish::headerFlags(uint8_t firstByte)
{
	this->retain(firstByte & 0x01);
	this->QoS((PktQoS)((firstByte & 0x06) >> 1));
	this->dup(firstByte & 0x08);

	if ((int)this->QoS() >= 0x03) {
		throw std::invalid_argument("Invalid QoS");
	}
}

enum StatusCode PktPublish::writeVariableHeader(AppBuf &buf)
{
	buf.writeBinary(this->topic());
	if (this->QoS() != PktQoS::QoS0) {
		buf.writeNum16(this->packetId());
	}

	if (properties.write(buf) == 0) {
		return StatusCode::PROPERTY_WRITE_ERROR;
	}

	return StatusCode::SUCCESS;
}

enum StatusCode PktPublish::writePayload(AppBuf &buf)
{
	if (this->payload().size() > 0) {
		buf.write(this->payload().data(), this->payload().size());
	}

	return StatusCode::SUCCESS;
}

uint32_t PktPublish::writeTo(AppBuf &buf)
{
	if (this->QoS() != PktQoS::QoS0 && this->packetId() == 0) {
		throw std::out_of_range("Invalid packet Id for this QoS level");
	}

	if (this->topic().size() == 0) {
		throw std::out_of_range("Invalid topic name");
	}

	Packet::fixedHeaderReserved = headerFlags();
	Packet::variableHeaderSize = stringLenSize + topic().size();
	if (this->QoS() != PktQoS::QoS0) {
		Packet::variableHeaderSize += 2;
	}
	Packet::payloadSize = this->payload().size();

	return Packet::writeTo(buf);
}

uint32_t PktPublish::readFrom(AppBuf &buf)
{
	std::size_t alreadyTraversed = buf.traversed();
	uint32_t remLen;
	uint8_t remLenWS;
	uint8_t first;
	StatusCode rc;

	if (buf.bytesToRead() < 4) {
		throw std::invalid_argument("Invalid input buffer");
	}

	first = buf.readNum8();
	if (m5::packetType(first) != PktType::PUBLISH) {
		throw std::invalid_argument("Invalid packet type");
	}

	headerFlags(first);

	rc = buf.readVBI(remLen, remLenWS);
	if (rc != StatusCode::SUCCESS) {
		return remLenWS;
	}

	if (remLen > buf.bytesToRead()) {
		throw std::out_of_range("No enough space in input buffer");
	}

	rc = buf.readBinary(this->_topic);
	if (rc != StatusCode::SUCCESS) {
		return buf.traversed() - alreadyTraversed;
	}

	if (this->topic().size() == 0) {
		throw std::invalid_argument("Invalid topic size");
	}

	if(this->QoS() != PktQoS::QoS0) {
		if (buf.bytesToRead() < 2) {
			throw std::invalid_argument("Invalid input buffer");
		}

		this->packetId(buf.readNum16());
		if (this->packetId() == 0) {
			throw std::invalid_argument("Invalid packet Id");
		}
	}

	properties.read(buf);

	const auto traversed = buf.traversed() - alreadyTraversed;
	const auto payloadSize = 1 + remLenWS + remLen - traversed;

	if (buf.bytesToRead() < payloadSize) {
		throw std::invalid_argument("Invalid input buffer");
	}

	buf.read(this->_payload, payloadSize);

	uint32_t fullPktSize = 1 + remLenWS + remLen;
	if (buf.traversed() - alreadyTraversed != fullPktSize) {
		throw std::invalid_argument("Corrupted input buffer");
	}

	return fullPktSize;
}

}

