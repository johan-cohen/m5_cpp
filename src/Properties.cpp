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

#include "Properties.hpp"

#include <stdexcept>
#include <cstring>

#define __POW2(n) (uint64_t)(((uint64_t)1) << (n))

namespace  m5 {

void Properties::computePktFlags(void)
{
	allowedProperties = 0;
	enabledProperties = 0;

	switch (pktType) {
	case PktType::CONNECT:
		allowedProperties += __POW2(PropertyId::SESSION_EXPIRY_INTERVAL);
		allowedProperties += __POW2(PropertyId::AUTH_METHOD);
		allowedProperties += __POW2(PropertyId::AUTH_DATA);
		allowedProperties += __POW2(PropertyId::REQUEST_PROBLEM_INFORMATION);
		allowedProperties += __POW2(PropertyId::WILL_DELAY_INTERVAL);
		allowedProperties += __POW2(PropertyId::REQUEST_RESPONSE_INFORMATION);
		allowedProperties += __POW2(PropertyId::RECEIVE_MAXIMUM);
		allowedProperties += __POW2(PropertyId::TOPIC_ALIAS_MAXIMUM);
		allowedProperties += __POW2(PropertyId::USER_PROPERTY);
		allowedProperties += __POW2(PropertyId::MAXIMUM_PACKET_SIZE);
		break;
	case PktType::CONNACK:
		allowedProperties += __POW2(PropertyId::ASSIGNED_CLIENT_IDENTIFIER);
		allowedProperties += __POW2(PropertyId::SERVER_KEEP_ALIVE);
		allowedProperties += __POW2(PropertyId::AUTH_METHOD);
		allowedProperties += __POW2(PropertyId::AUTH_DATA);
		allowedProperties += __POW2(PropertyId::RESPONSE_INFORMATION);
		allowedProperties += __POW2(PropertyId::SERVER_REFERENCE);
		allowedProperties += __POW2(PropertyId::REASON_STR);
		allowedProperties += __POW2(PropertyId::RECEIVE_MAXIMUM);
		allowedProperties += __POW2(PropertyId::TOPIC_ALIAS_MAXIMUM);
		allowedProperties += __POW2(PropertyId::MAXIMUM_QOS);
		allowedProperties += __POW2(PropertyId::RETAIN_AVAILABLE);
		allowedProperties += __POW2(PropertyId::USER_PROPERTY);
		allowedProperties += __POW2(PropertyId::MAXIMUM_PACKET_SIZE);
		allowedProperties += __POW2(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE);
		allowedProperties += __POW2(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE);
		allowedProperties += __POW2(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE);
		break;
	case PktType::PUBLISH:
		allowedProperties += __POW2(PropertyId::PAYLOAD_FORMAT_INDICATOR);
		allowedProperties += __POW2(PropertyId::PUBLICATION_EXPIRY_INTERVAL);
		allowedProperties += __POW2(PropertyId::CONTENT_TYPE);
		allowedProperties += __POW2(PropertyId::RESPONSE_TOPIC);
		allowedProperties += __POW2(PropertyId::CORRELATION_DATA);
		allowedProperties += __POW2(PropertyId::SUBSCRIPTION_IDENTIFIER);
		allowedProperties += __POW2(PropertyId::TOPIC_ALIAS);
		allowedProperties += __POW2(PropertyId::USER_PROPERTY);
		break;
	case PktType::PUBACK:
	case PktType::PUBREC:
	case PktType::PUBREL:
	case PktType::PUBCOMP:
	case PktType::SUBACK:
	case PktType::UNSUBACK:
		allowedProperties += __POW2(PropertyId::REASON_STR);
		allowedProperties += __POW2(PropertyId::USER_PROPERTY);
		break;
	case PktType::SUBSCRIBE:
		allowedProperties += __POW2(PropertyId::SUBSCRIPTION_IDENTIFIER);
		break;
	case PktType::UNSUBSCRIBE:
	case PktType::PINGREQ:
	case PktType::PINGRESP:
		break;
	case PktType::DISCONNECT:
		allowedProperties += __POW2(PropertyId::SESSION_EXPIRY_INTERVAL);
		allowedProperties += __POW2(PropertyId::SERVER_REFERENCE);
		allowedProperties += __POW2(PropertyId::REASON_STR);
		allowedProperties += __POW2(PropertyId::USER_PROPERTY);
		break;
	case PktType::AUTH:
		allowedProperties += __POW2(PropertyId::AUTH_METHOD);
		allowedProperties += __POW2(PropertyId::AUTH_DATA);
		allowedProperties += __POW2(PropertyId::REASON_STR);
		allowedProperties += __POW2(PropertyId::USER_PROPERTY);
		break;
	case PktType::RESERVED:
	default:
		break;
	}
}

Properties::Properties(const PktType type)
{
	this->resetPacketType(type);
}

Properties::~Properties()
{
}

void Properties::resetPacketType(const PktType type)
{
	this->pktType = type;
	this->computePktFlags();
}

bool Properties::isAllowed(uint8_t id) const
{
	return allowed() & __POW2(id);
}

bool Properties::isAllowed(PropertyId id) const
{
	return isAllowed((uint8_t)id);
}

bool Properties::isEnabled(PropertyId id) const
{
	return enabled() & __POW2(id);
}

void Properties::append(const ByteArray &key, const ByteArray &value)
{
	if (!isAllowed(PropertyId::USER_PROPERTY)) {
		throw std::invalid_argument("Invalid property for this packet");
	}

	userProps.push_back(KeyValuePair(key, value));
	enableProperty(PropertyId::USER_PROPERTY);
	this->_wireSize += propertyIdSize +
			   binaryLenSize + key.size() +
			   binaryLenSize + value.size();
}


void Properties::append(const uint8_t *key, uint16_t keySize,
			const uint8_t *value, uint16_t valueSize)
{
	ByteArray _key(key, key + keySize);
	ByteArray _value(value, value + valueSize);

	append(_key, _value);
}

void Properties::append(PropertyId id, const ByteArray &src)
{
	if (!isAllowed(id)) {
		throw std::invalid_argument("Invalid property for this packet");
	}

	if (isEnabled(id)) {
		auto it = binProps.find(id);
		if (it == binProps.end()) {
			throw std::out_of_range("Property enabled but not found");
		}

		ByteArray &item = (*it).second;
		this->_wireSize += -item.size() + src.size();

		item = src;

	} else {
		/* assume compiler will optimize this... */
		binProps.insert(BinaryPropPair(id, src));
		enableProperty(id);

		this->_wireSize += propertyIdSize + binaryLenSize + src.size();
	}
}

void Properties::append(PropertyId id, const uint8_t *data, uint16_t size)
{
	ByteArray src(data, data + size);
	append(id, src);
}

void Properties::append(PropertyId id, uint32_t value, uint32_t wireSize)
{
	if (!isAllowed(id)) {
		throw std::invalid_argument("Invalid property for this packet");
	}

	if (isEnabled(id)) {
		auto it = numProps.find(id);
		NumSize &numSize = (*it).second;

		numSize.num = value;
		numSize.size = wireSize;
	} else {
		struct NumSize numSize;

		numSize.num = value;
		numSize.size = wireSize;

		numProps.insert(NumPropPair((uint8_t)id, numSize));
		this->_wireSize += propertyIdSize + wireSize;
	}

	enableProperty(id);
}

const ByteArray &Properties::valueBinary(PropertyId id) const
{
	if (!isEnabled(id)) {
		static auto none = ByteArray();

		return none;
	}

	auto it = binProps.find(id);

	return (*it).second;
}

uint32_t Properties::valueNum(PropertyId id) const
{
	if (!isEnabled(id)) {
		return 0;
	}

	auto it = numProps.find(id);

	return ((*it).second).num;
}

void Properties::enableProperty(PropertyId id)
{
	this->enabledProperties |= __POW2(id);
}

void Properties::payloadFormatIndicator(uint8_t v)
{
	append(PropertyId::PAYLOAD_FORMAT_INDICATOR, v, 1);
}

uint8_t Properties::payloadFormatIndicator(void) const
{
	return valueNum(PropertyId::PAYLOAD_FORMAT_INDICATOR);
}

void Properties::publicationExpiryInterval(uint32_t v)
{
	append(PropertyId::PUBLICATION_EXPIRY_INTERVAL, v, 4);
}

uint32_t Properties::publicationExpiryInterval(void) const
{
	return valueNum(PropertyId::PUBLICATION_EXPIRY_INTERVAL);
}

void Properties::contentType(const uint8_t *data, uint16_t size)
{
	append(PropertyId::CONTENT_TYPE, data, size);
}

void Properties::contentType(const char *str)
{
	contentType((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::contentType(void) const
{
	return valueBinary(PropertyId::CONTENT_TYPE);
}

void Properties::responseTopic(const uint8_t *data, uint16_t size)
{
	append(PropertyId::RESPONSE_TOPIC, data, size);
}

void Properties::responseTopic(const char *str)
{
	responseTopic((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::responseTopic(void) const
{
	return valueBinary(PropertyId::RESPONSE_TOPIC);
}

void Properties::subscriptionIdentifier(uint32_t v)
{
	auto ws = VBIWireSize(v);
	append(PropertyId::SUBSCRIPTION_IDENTIFIER, v, ws);
}

uint32_t Properties::subscriptionIdentifier(void) const
{
	return valueNum(PropertyId::SUBSCRIPTION_IDENTIFIER);
}

void Properties::correlationData(const uint8_t *data, uint16_t size)
{
	append(PropertyId::CORRELATION_DATA, data, size);
}

const ByteArray &Properties::correlationData(void) const
{
	return valueBinary(PropertyId::CORRELATION_DATA);
}

void Properties::sessionExpiryInterval(uint32_t v)
{
	append(PropertyId::SESSION_EXPIRY_INTERVAL, v, 4);
}

uint32_t Properties::sessionExpiryInterval(void) const
{
	return valueNum(PropertyId::SESSION_EXPIRY_INTERVAL);
}

void Properties::assignedClientIdentifier(const uint8_t *data, uint16_t size)
{
	append(PropertyId::ASSIGNED_CLIENT_IDENTIFIER, data, size);
}

void Properties::assignedClientIdentifier(const char *str)
{
	assignedClientIdentifier((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::assignedClientIdentifier(void) const
{
	return valueBinary(PropertyId::ASSIGNED_CLIENT_IDENTIFIER);
}

void Properties::serverKeepAlive(uint16_t v)
{
	append(PropertyId::SERVER_KEEP_ALIVE, v, 2);
}

uint16_t Properties::serverKeepAlive(void) const
{
	return valueNum(PropertyId::SERVER_KEEP_ALIVE);
}

void Properties::authenticationMethod(const uint8_t *data, uint16_t size)
{
	append(PropertyId::AUTH_METHOD, data, size);
}

void Properties::authenticationMethod(const char *str)
{
	authenticationMethod((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::authenticationMethod(void) const
{
	return valueBinary(PropertyId::AUTH_METHOD);
}

void Properties::authenticationData(const uint8_t *data, uint16_t size)
{
	append(PropertyId::AUTH_DATA, data, size);
}

const ByteArray &Properties::authenticationData(void) const
{
	return valueBinary(PropertyId::AUTH_DATA);
}

void Properties::requestProblemInformation(bool v)
{
	append(PropertyId::REQUEST_PROBLEM_INFORMATION, v, 1);
}

bool Properties::requestProblemInformation(void) const
{
	return valueNum(PropertyId::REQUEST_PROBLEM_INFORMATION);
}

void Properties::willDelayInterval(uint32_t v)
{
	append(PropertyId::WILL_DELAY_INTERVAL, v, 4);
}

uint32_t Properties::willDelayInterval(void) const
{
	return valueNum(PropertyId::WILL_DELAY_INTERVAL);
}

void Properties::requestResponseInformation(bool v)
{
	append(PropertyId::REQUEST_RESPONSE_INFORMATION, v, 1);
}

bool Properties::requestResponseInformation(void) const
{
	return valueNum(PropertyId::REQUEST_RESPONSE_INFORMATION);
}

void Properties::responseInformation(const uint8_t *data, uint16_t size)
{
	append(PropertyId::RESPONSE_INFORMATION, data, size);
}

void Properties::responseInformation(const char *str)
{
	responseInformation((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::responseInformation(void) const
{
	return valueBinary(PropertyId::RESPONSE_INFORMATION);
}

void Properties::serverReference(const uint8_t *data, uint16_t size)
{
	append(PropertyId::SERVER_REFERENCE, data, size);
}

void Properties::serverReference(const char *str)
{
	serverReference((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::serverReference(void) const
{
	return valueBinary(PropertyId::SERVER_REFERENCE);
}

void Properties::reasonString(const uint8_t *data, uint16_t size)
{
	append(PropertyId::REASON_STR, data, size);
}

void Properties::reasonString(const char *str)
{
	reasonString((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::reasonString(void) const
{
	return valueBinary(PropertyId::REASON_STR);
}

void Properties::receiveMaximum(uint16_t v)
{
	append(PropertyId::RECEIVE_MAXIMUM, v, 2);
}

uint16_t Properties::receiveMaximum(void) const
{
	return valueNum(PropertyId::RECEIVE_MAXIMUM);
}

void Properties::topicAliasMaximum(uint16_t v)
{
	append(PropertyId::TOPIC_ALIAS_MAXIMUM, v, 2);
}

uint16_t Properties::topicAliasMaximum(void) const
{
	return valueNum(PropertyId::TOPIC_ALIAS_MAXIMUM);
}

void Properties::topicAlias(uint16_t v)
{
	append(PropertyId::TOPIC_ALIAS, v, 2);
}

uint16_t Properties::topicAlias(void) const
{
	return valueNum(PropertyId::TOPIC_ALIAS);
}

void Properties::maximumQoS(PktQoS qos)
{
	switch (qos) {
	case PktQoS::QoS0:
	case PktQoS::QoS1:
	case PktQoS::QoS2:
		break;
	default:
		throw std::invalid_argument("Invalid QoS value");
	}

	append(PropertyId::MAXIMUM_QOS, (uint8_t)qos, 1);
}

PktQoS Properties::maximumQoS(void) const
{
	return (PktQoS)valueNum(PropertyId::MAXIMUM_QOS);
}

void Properties::retainAvailable(bool v)
{
	append(PropertyId::RETAIN_AVAILABLE, v, 1);
}

bool Properties::retainAvailable(void) const
{
	return valueNum(PropertyId::RETAIN_AVAILABLE);
}

void Properties::userProperty(const uint8_t *key, uint16_t keySize,
				  const uint8_t *value, uint16_t valueSize)
{
	append(key, keySize, value, valueSize);
}

void Properties::userProperty(const char *key, const char *val)
{
	userProperty((const uint8_t *)key, strlen(key),
		     (const uint8_t *)val, strlen(val));
}

const UserProperty &Properties::userProperty(void) const
{
	return userProps;
}

void Properties::maximumPacketSize(uint32_t v)
{
	append(PropertyId::MAXIMUM_PACKET_SIZE, v, 4);
}

uint32_t Properties::maximumPacketSize(void) const
{
	return valueNum(PropertyId::MAXIMUM_PACKET_SIZE);
}

void Properties::wildcardSubscriptionAvailable(bool v)
{
	append(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE, v, 1);
}

bool Properties::wildcardSubscriptionAvailable(void) const
{
	return valueNum(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE);
}

void Properties::subscriptionIdentifierAvailable(bool v)
{
	append(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE, v, 1);
}

bool Properties::subscriptionIdentifierAvailable(void) const
{
	return valueNum(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE);
}

void Properties::sharedSubscriptionAvailable(bool v)
{
	append(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE, v, 1);
}

bool Properties::sharedSubscriptionAvailable(void) const
{
	return valueNum(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE);
}

uint32_t Properties::read(AppBuf &buf)
{
	uint32_t propWireSize;
	uint32_t fieldLen;
	uint32_t number;
	ByteArray value;
	ByteArray key;

	propWireSize = buf.readVBI();
	if (this->wireSize() >  buf.bytesToRead()) {
		throw std::invalid_argument("Invalid input buffer");
	}

	const uint32_t bytesAtBeginning = buf.bytesToRead();
	while (bytesAtBeginning - buf.bytesToRead() < propWireSize) {
		auto id = buf.readNum8();

		switch (id) {
		case PAYLOAD_FORMAT_INDICATOR:
		case REQUEST_PROBLEM_INFORMATION:
		case REQUEST_RESPONSE_INFORMATION:
		case MAXIMUM_QOS:
		case RETAIN_AVAILABLE:
		case WILDCARD_SUBSCRIPTION_AVAILABLE:
		case SUBSCRIPTION_IDENTIFIER_AVAILABLE:
		case SHARED_SUBSCRIPTION_AVAILABLE:
			if (buf.bytesToRead() < 1) {
				throw std::invalid_argument("Invalid input buffer");
			}

			number = buf.readNum8();
			fieldLen = 1;
			this->append((PropertyId)id, number, fieldLen);
			break;

		case SERVER_KEEP_ALIVE:
		case RECEIVE_MAXIMUM:
		case TOPIC_ALIAS_MAXIMUM:
		case TOPIC_ALIAS:
			if (buf.bytesToRead() < 2) {
				throw std::invalid_argument("Invalid input buffer");
			}

			number = buf.readNum16();
			fieldLen = 2;
			this->append((PropertyId)id, number, fieldLen);
			break;

		case PUBLICATION_EXPIRY_INTERVAL:
		case SESSION_EXPIRY_INTERVAL:
		case WILL_DELAY_INTERVAL:
		case MAXIMUM_PACKET_SIZE:
			if (buf.bytesToRead() < 4) {
				throw std::invalid_argument("Invalid input buffer");
			}

			number = buf.readNum32();
			fieldLen = 4;
			this->append((PropertyId)id, number, fieldLen);
			break;

		case CONTENT_TYPE:
		case RESPONSE_TOPIC:
		case CORRELATION_DATA:
		case ASSIGNED_CLIENT_IDENTIFIER:
		case AUTH_METHOD:
		case AUTH_DATA:
		case RESPONSE_INFORMATION:
		case SERVER_REFERENCE:
		case REASON_STR:
			buf.readBinary(value);
			this->append((PropertyId)id, value);
			break;

		case SUBSCRIPTION_IDENTIFIER:
			this->append((PropertyId)id, buf.readVBI(), VBIWireSize(number));
			break;

		case USER_PROPERTY:
			/* very inefficient way to solve this... */
			buf.readKeyValue(key, value);
			this->append(key, value);
			break;
		default:
			throw std::invalid_argument("Invalid Property Id");
		}
	}

	/* bytes read is different to wire size */
	if (propWireSize != this->wireSize()) {
		throw std::invalid_argument("Invalid input buffer");
	}

	return this->wireSize();
}


static void writeNumProp(AppBuf &buf, uint32_t num, uint8_t size)
{
	typedef void (AppBuf::*WriteNumPtr)(uint32_t);
	WriteNumPtr ptrs[] = { &AppBuf::writeNum8, &AppBuf::writeNum16, &AppBuf::writeNum32 };

	WriteNumPtr ptr = ptrs[size / 2];

	(buf.*ptr)(num);
}

uint32_t Properties::write(AppBuf &buf)
{
	auto fullLen = this->wireSize() + VBIWireSize(this->wireSize());
	if (fullLen > buf.bytesToWrite()) {
		throw std::out_of_range("No enough space in input buffer");
	}

	buf.writeVBI(this->wireSize());

	auto itNum = numProps.begin();
	while (itNum != numProps.end()) {
		auto id = (*itNum).first;
		auto num = (*itNum).second;

		buf.writeNum8(id);
		writeNumProp(buf, num.num, num.size);

		itNum++;
	}

	auto itBin = binProps.begin();
	while (itBin != binProps.end()) {
		auto id = (*itBin).first;
		auto &item = (*itBin).second;

		buf.writeNum8(id);
		buf.writeBinary(item.data(), item.size());

		itBin++;
	}

	auto itUser = userProps.begin();
	while (itUser != userProps.end()) {
		auto &key = (*itUser).first;
		auto &value = (*itUser).second;

		buf.writeNum8(PropertyId::USER_PROPERTY);
		buf.writeBinary(key.data(), key.size());
		buf.writeBinary(value.data(), value.size());

		itUser++;
	}

	return this->_wireSize;
}

}
