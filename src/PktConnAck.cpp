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

void PktConnAck::assignedClientIdentifier(const uint8_t *data, uint16_t size)
{
	properties.assignedClientIdentifier(data, size);
}

void PktConnAck::assignedClientIdentifier(const char *str)
{
	properties.assignedClientIdentifier(str);
}

const ByteArray &PktConnAck::assignedClientIdentifier(void) const
{
	return properties.assignedClientIdentifier();
}

void PktConnAck::serverKeepAlive(uint16_t v)
{
	properties.serverKeepAlive(v);
}

uint16_t PktConnAck::serverKeepAlive(void) const
{
	return properties.serverKeepAlive();
}

void PktConnAck::authenticationMethod(const uint8_t *data, uint16_t size)
{
	properties.authenticationMethod(data, size);
}

void PktConnAck::authenticationMethod(const char *str)
{
	properties.authenticationMethod(str);
}

const ByteArray &PktConnAck::authenticationMethod(void) const
{
	return properties.authenticationMethod();
}

void PktConnAck::authenticationData(const uint8_t *data, uint16_t size)
{
	properties.authenticationData(data, size);
}

const ByteArray &PktConnAck::authenticationData(void) const
{
	return properties.authenticationData();
}

void PktConnAck::responseInformation(const uint8_t *data, uint16_t size)
{
	properties.responseInformation(data, size);
}

void PktConnAck::responseInformation(const char *str)
{
	properties.responseInformation(str);
}

const ByteArray &PktConnAck::responseInformation(void) const
{
	return properties.responseInformation();
}

void PktConnAck::serverReference(const uint8_t *data, uint16_t size)
{
	properties.serverReference(data, size);
}

void PktConnAck::serverReference(const char *str)
{
	properties.serverReference(str);
}

const ByteArray &PktConnAck::serverReference(void) const
{
	return properties.serverReference();
}

void PktConnAck::reasonString(const uint8_t *data, uint16_t size)
{
	properties.reasonString(data, size);
}

void PktConnAck::reasonString(const char *str)
{
	properties.reasonString(str);
}

const ByteArray &PktConnAck::reasonString(void) const
{
	return properties.reasonString();
}

void PktConnAck::receiveMaximum(uint16_t v)
{
	properties.receiveMaximum(v);
}

uint16_t PktConnAck::receiveMaximum(void) const
{
	return properties.receiveMaximum();
}

void PktConnAck::topicAliasMaximum(uint16_t v)
{
	properties.topicAliasMaximum(v);
}

uint16_t PktConnAck::topicAliasMaximum(void) const
{
	return properties.topicAliasMaximum();
}

void PktConnAck::maximumQoS(PktQoS qos)
{
	properties.maximumQoS(qos);
}

PktQoS PktConnAck::maximumQoS(void) const
{
	return properties.maximumQoS();
}

void PktConnAck::retainAvailable(bool v)
{
	properties.retainAvailable(v);
}

bool PktConnAck::retainAvailable(void) const
{
	return properties.retainAvailable();
}

void PktConnAck::userProperty(const uint8_t *key, uint16_t keySize,
			      const uint8_t *value, uint16_t valueSize)
{
	properties.userProperty(key, keySize, value, valueSize);
}

void PktConnAck::userProperty(const char *key, const char *val)
{
	properties.userProperty(key, val);
}

const UserProperty &PktConnAck::userProperty(void) const
{
	return properties.userProperty();
}

void PktConnAck::maximumPacketSize(uint32_t v)
{
	properties.maximumPacketSize(v);
}

uint32_t PktConnAck::maximumPacketSize(void) const
{
	return properties.maximumPacketSize();
}

void PktConnAck::wildcardSubscriptionAvailable(bool v)
{
	properties.wildcardSubscriptionAvailable(v);
}

bool PktConnAck::wildcardSubscriptionAvailable(void) const
{
	return properties.wildcardSubscriptionAvailable();
}

void PktConnAck::subscriptionIdentifierAvailable(bool v)
{
	properties.subscriptionIdentifierAvailable(v);
}

bool PktConnAck::subscriptionIdentifierAvailable(void) const
{
	return properties.subscriptionIdentifierAvailable();
}

void PktConnAck::sharedSubscriptionAvailable(bool v)
{
	properties.sharedSubscriptionAvailable(v);
}

bool PktConnAck::sharedSubscriptionAvailable(void) const
{
	return properties.sharedSubscriptionAvailable();
}

uint32_t PktConnAck::writeTo(AppBuf &buf)
{
	const auto initialLength = buf.length();
	uint32_t fullPktSize;
	uint32_t propWSWS;
	uint32_t propWS;
	uint32_t remLenWS;
	uint32_t remLen;

	status(StatusCode::SUCCESS);
	expectedWireSize(0);

	propWS = properties.wireSize();
	propWSWS = VBIWireSize(propWS);
	if (propWSWS == 0) {
		status(StatusCode::INVALID_PROPERTY_VBI);
		goto lb_exit;
	}

	remLen = 2 + propWSWS + propWS;
	remLenWS = VBIWireSize(remLen);
	if (remLenWS == 0) {
		status(StatusCode::INVALID_REMLEN_VBI);
		goto lb_exit;
	}

	fullPktSize = 1 + remLenWS + remLen;
	expectedWireSize(fullPktSize);
	if (buf.bytesToWrite() < expectedWireSize()) {
		status(StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER);
		goto lb_exit;
	}

	buf.writeNum8(m5::firstByte(PktType::CONNACK));
	buf.writeVBI(remLen);
	buf.writeNum8(this->_sessionPresent ? 0x01 : 0x00);
	buf.writeNum8(this->_reasonCode);
	if (properties.write(buf) != propWSWS + propWS) {
		status(StatusCode::PROPERTY_WRITE_ERROR);
	}

lb_exit:
	return buf.length() - initialLength;
}

uint32_t PktConnAck::readFrom(AppBuf &buf)
{
	std::size_t alreadyTraversed = buf.traversed();
	uint32_t fullPktSize;
	uint8_t remLenWS;
	uint32_t remLen;
	uint8_t number;
	int rc;

	status(StatusCode::SUCCESS);
	expectedWireSize(0);

	if (buf.bytesToRead() < 5) {
		status(StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER);
		goto lb_exit;
	}

	number = buf.readNum8();
	if (number != ((uint8_t)PktType::CONNACK << 4)) {
		status(StatusCode::INVALID_FIXED_HEADER);
		goto lb_exit;
	}

	rc = buf.readVBI(remLen, remLenWS);
	if (rc != EXIT_SUCCESS) {
		status(StatusCode::INVALID_REMLEN_VBI);
		goto lb_exit;
	}

	if (remLen < 3) {
		status(StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER);
		goto lb_exit;
	}

	number = buf.readNum8();
	if (number & 0xFD) {
		status(StatusCode::RESERVED_MUST_BE_ZERO);
		goto lb_exit;
	}
	this->_sessionPresent = number ? true : false;
	this->_reasonCode = buf.readNum8();
	properties.read(buf);

	fullPktSize = 1 + remLenWS + remLen;
	expectedWireSize(fullPktSize);
	if (buf.traversed() - alreadyTraversed != expectedWireSize()) {
		status(StatusCode::FINISHED_READING_INVALID_LENGTH);
	}

lb_exit:
	return buf.traversed() - alreadyTraversed;
}

}
