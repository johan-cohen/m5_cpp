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

void PropertiesList::computePktFlags(void)
{
	properties = 0;
	enabledProperties = 0;

	switch (pktType) {
	case PktType::CONNECT:
		properties += __POW2(PropertyId::SESSION_EXPIRY_INTERVAL);
		properties += __POW2(PropertyId::AUTH_METHOD);
		properties += __POW2(PropertyId::AUTH_DATA);
		properties += __POW2(PropertyId::REQUEST_PROBLEM_INFORMATION);
		properties += __POW2(PropertyId::WILL_DELAY_INTERVAL);
		properties += __POW2(PropertyId::REQUEST_RESPONSE_INFORMATION);
		properties += __POW2(PropertyId::RECEIVE_MAXIMUM);
		properties += __POW2(PropertyId::TOPIC_ALIAS_MAXIMUM);
		properties += __POW2(PropertyId::USER_PROPERTY);
		properties += __POW2(PropertyId::MAXIMUM_PACKET_SIZE);
		break;
	case PktType::CONNACK:
		properties += __POW2(PropertyId::ASSIGNED_CLIENT_IDENTIFIER);
		properties += __POW2(PropertyId::SERVER_KEEP_ALIVE);
		properties += __POW2(PropertyId::AUTH_METHOD);
		properties += __POW2(PropertyId::AUTH_DATA);
		properties += __POW2(PropertyId::RESPONSE_INFORMATION);
		properties += __POW2(PropertyId::SERVER_REFERENCE);
		properties += __POW2(PropertyId::REASON_STR);
		properties += __POW2(PropertyId::RECEIVE_MAXIMUM);
		properties += __POW2(PropertyId::TOPIC_ALIAS_MAXIMUM);
		properties += __POW2(PropertyId::MAXIMUM_QOS);
		properties += __POW2(PropertyId::RETAIN_AVAILABLE);
		properties += __POW2(PropertyId::USER_PROPERTY);
		properties += __POW2(PropertyId::MAXIMUM_PACKET_SIZE);
		properties += __POW2(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE);
		properties += __POW2(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE);
		properties += __POW2(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE);
		break;
	case PktType::PUBLISH:
		properties += __POW2(PropertyId::PAYLOAD_FORMAT_INDICATOR);
		properties += __POW2(PropertyId::PUBLICATION_EXPIRY_INTERVAL);
		properties += __POW2(PropertyId::CONTENT_TYPE);
		properties += __POW2(PropertyId::RESPONSE_TOPIC);
		properties += __POW2(PropertyId::CORRELATION_DATA);
		properties += __POW2(PropertyId::SUBSCRIPTION_IDENTIFIER);
		properties += __POW2(PropertyId::TOPIC_ALIAS);
		properties += __POW2(PropertyId::USER_PROPERTY);
		break;
	case PktType::PUBACK:
	case PktType::PUBREC:
	case PktType::PUBREL:
	case PktType::PUBCOMP:
	case PktType::SUBACK:
	case PktType::UNSUBACK:
		properties += __POW2(PropertyId::REASON_STR);
		properties += __POW2(PropertyId::USER_PROPERTY);
		break;
	case PktType::SUBSCRIBE:
		properties += __POW2(PropertyId::SUBSCRIPTION_IDENTIFIER);
		break;
	case PktType::UNSUBSCRIBE:
	case PktType::PINGREQ:
	case PktType::PINGRESP:
		break;
	case PktType::DISCONNECT:
		properties += __POW2(PropertyId::SESSION_EXPIRY_INTERVAL);
		properties += __POW2(PropertyId::SERVER_REFERENCE);
		properties += __POW2(PropertyId::REASON_STR);
		properties += __POW2(PropertyId::USER_PROPERTY);
		break;
	case PktType::AUTH:
		properties += __POW2(PropertyId::AUTH_METHOD);
		properties += __POW2(PropertyId::AUTH_DATA);
		properties += __POW2(PropertyId::REASON_STR);
		properties += __POW2(PropertyId::USER_PROPERTY);
		break;
	case PktType::RESERVED:
	default:
		break;
	}
}

PropertiesList::PropertiesList(const PktType type)
{
	this->resetPacketType(type);
}

PropertiesList::~PropertiesList()
{
}

void PropertiesList::resetPacketType(const PktType type)
{
	this->pktType = type;
	this->computePktFlags();
}

bool PropertiesList::isAllowed(uint8_t id) const
{
	return allowed() & __POW2(id);
}

bool PropertiesList::isAllowed(PropertyId id) const
{
	return isAllowed((uint8_t)id);
}

bool PropertiesList::isEnabled(PropertyId id) const
{
	return enabled() & __POW2(id);
}

void PropertiesList::append(const ByteArray &key, const ByteArray &value)
{
	if (!isAllowed(PropertyId::USER_PROPERTY)) {
		return;
	}

	userProps.push_back(KeyValuePair(key, value));
	enableProperty(PropertyId::USER_PROPERTY);
	this->_wireSize += propertyIdSize +
			   binaryLenSize + key.size() +
			   binaryLenSize + value.size();
}


void PropertiesList::append(const uint8_t *key, uint16_t keySize,
			    const uint8_t *value, uint16_t valueSize)
{
	ByteArray _key(key, key + keySize);
	ByteArray _value(value, value + valueSize);

	append(_key, _value);
}

void PropertiesList::append(PropertyId id, const ByteArray &src)
{
	if (!isAllowed(id)) {
		return;
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

void PropertiesList::append(PropertyId id, const uint8_t *data, uint16_t size)
{
	ByteArray src(data, data + size);
	append(id, src);
}

void PropertiesList::append(PropertyId id, uint32_t value, uint32_t wireSize)
{
	if (!isAllowed(id)) {
		return;
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

const ByteArray &PropertiesList::valueBinary(PropertyId id) const
{
	if (!isEnabled(id)) {
		static auto none = ByteArray();

		return none;
	}

	auto it = binProps.find(id);

	return (*it).second;
}

uint32_t PropertiesList::valueNum(PropertyId id) const
{
	if (!isEnabled(id)) {
		return 0;
	}

	auto it = numProps.find(id);

	return ((*it).second).num;
}

void PropertiesList::enableProperty(PropertyId id)
{
	this->enabledProperties |= __POW2(id);
}

void PropertiesList::payloadFormatIndicator(uint8_t v)
{
	append(PropertyId::PAYLOAD_FORMAT_INDICATOR, v, 1);
}

uint8_t PropertiesList::payloadFormatIndicator(void) const
{
	return valueNum(PropertyId::PAYLOAD_FORMAT_INDICATOR);
}

void PropertiesList::publicationExpiryInterval(uint32_t v)
{
	append(PropertyId::PUBLICATION_EXPIRY_INTERVAL, v, 4);
}

uint32_t PropertiesList::publicationExpiryInterval(void) const
{
	return valueNum(PropertyId::PUBLICATION_EXPIRY_INTERVAL);
}

void PropertiesList::contentType(const uint8_t *data, uint16_t size)
{
	append(PropertyId::CONTENT_TYPE, data, size);
}

void PropertiesList::contentType(const char *str)
{
	contentType((const uint8_t *)str, strlen(str));
}

const ByteArray &PropertiesList::contentType(void) const
{
	return valueBinary(PropertyId::CONTENT_TYPE);
}

void PropertiesList::responseTopic(const uint8_t *data, uint16_t size)
{
	append(PropertyId::RESPONSE_TOPIC, data, size);
}

void PropertiesList::responseTopic(const char *str)
{
	responseTopic((const uint8_t *)str, strlen(str));
}

const ByteArray &PropertiesList::responseTopic(void) const
{
	return valueBinary(PropertyId::RESPONSE_TOPIC);
}

void PropertiesList::subscriptionIdentifier(uint32_t v)
{
	auto ws = VBIWireSize(v);
	append(PropertyId::SUBSCRIPTION_IDENTIFIER, v, ws);
}

uint32_t PropertiesList::subscriptionIdentifier(void) const
{
	return valueNum(PropertyId::SUBSCRIPTION_IDENTIFIER);
}

void PropertiesList::correlationData(const uint8_t *data, uint16_t size)
{
	append(PropertyId::CORRELATION_DATA, data, size);
}

const ByteArray &PropertiesList::correlationData(void) const
{
	return valueBinary(PropertyId::CORRELATION_DATA);
}

void PropertiesList::sessionExpiryInterval(uint32_t v)
{
	append(PropertyId::SESSION_EXPIRY_INTERVAL, v, 4);
}

uint32_t PropertiesList::sessionExpiryInterval(void) const
{
	return valueNum(PropertyId::SESSION_EXPIRY_INTERVAL);
}

void PropertiesList::assignedClientIdentifier(const uint8_t *data, uint16_t size)
{
	append(PropertyId::ASSIGNED_CLIENT_IDENTIFIER, data, size);
}

void PropertiesList::assignedClientIdentifier(const char *str)
{
	assignedClientIdentifier((const uint8_t *)str, strlen(str));
}

const ByteArray &PropertiesList::assignedClientIdentifier(void) const
{
	return valueBinary(PropertyId::ASSIGNED_CLIENT_IDENTIFIER);
}

void PropertiesList::serverKeepAlive(uint16_t v)
{
	append(PropertyId::SERVER_KEEP_ALIVE, v, 2);
}

uint16_t PropertiesList::serverKeepAlive(void) const
{
	return valueNum(PropertyId::SERVER_KEEP_ALIVE);
}

void PropertiesList::authenticationMethod(const uint8_t *data, uint16_t size)
{
	append(PropertyId::AUTH_METHOD, data, size);
}

void PropertiesList::authenticationMethod(const char *str)
{
	authenticationMethod((const uint8_t *)str, strlen(str));
}

const ByteArray &PropertiesList::authenticationMethod(void) const
{
	return valueBinary(PropertyId::AUTH_METHOD);
}

void PropertiesList::authenticationData(const uint8_t *data, uint16_t size)
{
	append(PropertyId::AUTH_DATA, data, size);
}

const ByteArray &PropertiesList::authenticationData(void) const
{
	return valueBinary(PropertyId::AUTH_DATA);
}

void PropertiesList::requestProblemInformation(bool v)
{
	append(PropertyId::REQUEST_PROBLEM_INFORMATION, v, 1);
}

bool PropertiesList::requestProblemInformation(void) const
{
	return valueNum(PropertyId::REQUEST_PROBLEM_INFORMATION);
}

void PropertiesList::willDelayInterval(uint32_t v)
{
	append(PropertyId::WILL_DELAY_INTERVAL, v, 4);
}

uint32_t PropertiesList::willDelayInterval(void) const
{
	return valueNum(PropertyId::WILL_DELAY_INTERVAL);
}

void PropertiesList::requestResponseInformation(bool v)
{
	append(PropertyId::REQUEST_RESPONSE_INFORMATION, v, 1);
}

bool PropertiesList::requestResponseInformation(void) const
{
	return valueNum(PropertyId::REQUEST_RESPONSE_INFORMATION);
}

void PropertiesList::responseInformation(const uint8_t *data, uint16_t size)
{
	append(PropertyId::RESPONSE_INFORMATION, data, size);
}

void PropertiesList::responseInformation(const char *str)
{
	responseInformation((const uint8_t *)str, strlen(str));
}

const ByteArray &PropertiesList::responseInformation(void) const
{
	return valueBinary(PropertyId::RESPONSE_INFORMATION);
}

void PropertiesList::serverReference(const uint8_t *data, uint16_t size)
{
	append(PropertyId::SERVER_REFERENCE, data, size);
}

void PropertiesList::serverReference(const char *str)
{
	serverReference((const uint8_t *)str, strlen(str));
}

const ByteArray &PropertiesList::serverReference(void) const
{
	return valueBinary(PropertyId::SERVER_REFERENCE);
}

void PropertiesList::reasonString(const uint8_t *data, uint16_t size)
{
	append(PropertyId::REASON_STR, data, size);
}

void PropertiesList::reasonString(const char *str)
{
	reasonString((const uint8_t *)str, strlen(str));
}

const ByteArray &PropertiesList::reasonString(void) const
{
	return valueBinary(PropertyId::REASON_STR);
}

void PropertiesList::receiveMaximum(uint16_t v)
{
	append(PropertyId::RECEIVE_MAXIMUM, v, 2);
}

uint16_t PropertiesList::receiveMaximum(void) const
{
	return valueNum(PropertyId::RECEIVE_MAXIMUM);
}

void PropertiesList::topicAliasMaximum(uint16_t v)
{
	append(PropertyId::TOPIC_ALIAS_MAXIMUM, v, 2);
}

uint16_t PropertiesList::topicAliasMaximum(void) const
{
	return valueNum(PropertyId::TOPIC_ALIAS_MAXIMUM);
}

void PropertiesList::topicAlias(uint16_t v)
{
	append(PropertyId::TOPIC_ALIAS, v, 2);
}

uint16_t PropertiesList::topicAlias(void) const
{
	return valueNum(PropertyId::TOPIC_ALIAS);
}

void PropertiesList::maximumQoS(PktQoS qos)
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

PktQoS PropertiesList::maximumQoS(void) const
{
	return (PktQoS)valueNum(PropertyId::MAXIMUM_QOS);
}

void PropertiesList::retainAvailable(bool v)
{
	append(PropertyId::RETAIN_AVAILABLE, v, 1);
}

bool PropertiesList::retainAvailable(void) const
{
	return valueNum(PropertyId::RETAIN_AVAILABLE);
}

void PropertiesList::userProperty(const uint8_t *key, uint16_t keySize,
				  const uint8_t *value, uint16_t valueSize)
{
	append(key, keySize, value, valueSize);
}

void PropertiesList::userProperty(const char *key, const char *val)
{
	userProperty((const uint8_t *)key, strlen(key),
		     (const uint8_t *)val, strlen(val));
}

const UserProperty &PropertiesList::userProperty(void) const
{
	return userProps;
}

void PropertiesList::maximumPacketSize(uint32_t v)
{
	append(PropertyId::MAXIMUM_PACKET_SIZE, v, 4);
}

uint32_t PropertiesList::maximumPacketSize(void) const
{
	return valueNum(PropertyId::MAXIMUM_PACKET_SIZE);
}

void PropertiesList::wildcardSubscriptionAvailable(bool v)
{
	append(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE, v, 1);
}

bool PropertiesList::wildcardSubscriptionAvailable(void) const
{
	return valueNum(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE);
}

void PropertiesList::subscriptionIdentifierAvailable(bool v)
{
	append(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE, v, 1);
}

bool PropertiesList::subscriptionIdentifierAvailable(void) const
{
	return valueNum(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE);
}

void PropertiesList::sharedSubscriptionAvailable(bool v)
{
	append(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE, v, 1);
}

bool PropertiesList::sharedSubscriptionAvailable(void) const
{
	return valueNum(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE);
}

uint32_t PropertiesList::read(AppBuf &buf)
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

uint32_t PropertiesList::write(AppBuf &buf)
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
		switch (num.size) {
		case 1:
			buf.writeNum8(num.num);
			break;
		case 2:
			buf.writeNum16(num.num);
			break;
		case 4:
			buf.writeNum32(num.num);
			break;
		default:
			throw std::invalid_argument("Invalid integer size");
		}

		itNum++;
	}

	auto itBin = binProps.begin();
	while (itBin != binProps.end()) {
		auto id = (*itBin).first;
		auto &item = (*itBin).second;

		buf.writeNum8(id);
		buf.writeBinary(&item[0], item.size());

		itBin++;
	}

	auto itUser = userProps.begin();
	while (itUser != userProps.end()) {
		auto &key = (*itUser).first;
		auto &value = (*itUser).second;

		buf.writeNum8(PropertyId::USER_PROPERTY);
		buf.writeBinary(&key[0], key.size());
		buf.writeBinary(&value[0], value.size());

		itUser++;
	}

	return this->_wireSize;
}

}
