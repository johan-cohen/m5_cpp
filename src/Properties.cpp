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

#include <cstring>

namespace  m5 {

Properties::Properties(const PktType type)
{
	this->resetPacketType(type);
}

Properties::~Properties()
{
	auto itBin = binProps.begin();
	while (itBin != binProps.end())
	{
		auto pair = *itBin;
		ByteArray *value = pair.second;

		delete value;

		itBin++;
	}

	auto itUser = userProps.begin();
	while (itUser != userProps.end()) {
		ByteArray *key = (*itUser).first;
		ByteArray *value = (*itUser).second;

		delete key;
		delete value;

		itUser++;
	}
}

void Properties::resetPacketType(const PktType type)
{
	this->pktType = type;
	this->computePktFlags();
}

static uint64_t pow2(enum PropertyId id)
{
	return (uint64_t)0x01 << (uint8_t)id;
}

void Properties::computePktFlags(void)
{
	allowedProperties = 0;
	enabledProperties = 0;

	switch (pktType) {
	case PktType::CONNECT:
		allowedProperties += pow2(PropertyId::SESSION_EXPIRY_INTERVAL);
		allowedProperties += pow2(PropertyId::AUTH_METHOD);
		allowedProperties += pow2(PropertyId::AUTH_DATA);
		allowedProperties += pow2(PropertyId::REQUEST_PROBLEM_INFORMATION);
		allowedProperties += pow2(PropertyId::WILL_DELAY_INTERVAL);
		allowedProperties += pow2(PropertyId::REQUEST_RESPONSE_INFORMATION);
		allowedProperties += pow2(PropertyId::RECEIVE_MAXIMUM);
		allowedProperties += pow2(PropertyId::TOPIC_ALIAS_MAXIMUM);
		allowedProperties += pow2(PropertyId::USER_PROPERTY);
		allowedProperties += pow2(PropertyId::MAXIMUM_PACKET_SIZE);
		break;
	case PktType::CONNACK:
		allowedProperties += pow2(PropertyId::ASSIGNED_CLIENT_IDENTIFIER);
		allowedProperties += pow2(PropertyId::SERVER_KEEP_ALIVE);
		allowedProperties += pow2(PropertyId::AUTH_METHOD);
		allowedProperties += pow2(PropertyId::AUTH_DATA);
		allowedProperties += pow2(PropertyId::RESPONSE_INFORMATION);
		allowedProperties += pow2(PropertyId::SERVER_REFERENCE);
		allowedProperties += pow2(PropertyId::REASON_STR);
		allowedProperties += pow2(PropertyId::RECEIVE_MAXIMUM);
		allowedProperties += pow2(PropertyId::TOPIC_ALIAS_MAXIMUM);
		allowedProperties += pow2(PropertyId::MAXIMUM_QOS);
		allowedProperties += pow2(PropertyId::RETAIN_AVAILABLE);
		allowedProperties += pow2(PropertyId::USER_PROPERTY);
		allowedProperties += pow2(PropertyId::MAXIMUM_PACKET_SIZE);
		allowedProperties += pow2(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE);
		allowedProperties += pow2(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE);
		allowedProperties += pow2(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE);
		break;
	case PktType::PUBLISH:
		allowedProperties += pow2(PropertyId::PAYLOAD_FORMAT_INDICATOR);
		allowedProperties += pow2(PropertyId::PUBLICATION_EXPIRY_INTERVAL);
		allowedProperties += pow2(PropertyId::CONTENT_TYPE);
		allowedProperties += pow2(PropertyId::RESPONSE_TOPIC);
		allowedProperties += pow2(PropertyId::CORRELATION_DATA);
		allowedProperties += pow2(PropertyId::SUBSCRIPTION_IDENTIFIER);
		allowedProperties += pow2(PropertyId::TOPIC_ALIAS);
		allowedProperties += pow2(PropertyId::USER_PROPERTY);
		break;
	case PktType::PUBACK:
	case PktType::PUBREC:
	case PktType::PUBREL:
	case PktType::PUBCOMP:
	case PktType::SUBACK:
	case PktType::UNSUBACK:
		allowedProperties += pow2(PropertyId::REASON_STR);
		allowedProperties += pow2(PropertyId::USER_PROPERTY);
		break;
	case PktType::SUBSCRIBE:
		allowedProperties += pow2(PropertyId::SUBSCRIPTION_IDENTIFIER);
		break;
	case PktType::UNSUBSCRIBE:
	case PktType::PINGREQ:
	case PktType::PINGRESP:
		break;
	case PktType::DISCONNECT:
		allowedProperties += pow2(PropertyId::SESSION_EXPIRY_INTERVAL);
		allowedProperties += pow2(PropertyId::SERVER_REFERENCE);
		allowedProperties += pow2(PropertyId::REASON_STR);
		allowedProperties += pow2(PropertyId::USER_PROPERTY);
		break;
	case PktType::AUTH:
		allowedProperties += pow2(PropertyId::AUTH_METHOD);
		allowedProperties += pow2(PropertyId::AUTH_DATA);
		allowedProperties += pow2(PropertyId::REASON_STR);
		allowedProperties += pow2(PropertyId::USER_PROPERTY);
		break;
	case PktType::RESERVED:
	default:
		break;
	}
}

bool Properties::isAllowed(PropertyId id) const
{
	return allowed() & pow2(id);
}

bool Properties::isEnabled(PropertyId id) const
{
	return enabled() & pow2(id);
}

enum StatusCode Properties::append(ByteArray *key, ByteArray *value)
{
	if (!isAllowed(PropertyId::USER_PROPERTY)) {
		return StatusCode::INVALID_PROPERTY;
	}

	userProps.push_back(KeyValuePair(key, value));
	enableProperty(PropertyId::USER_PROPERTY);
	this->_wireSize += propertyIdSize +
			   binaryLenSize + key->size() +
			   binaryLenSize + value->size();

	return StatusCode::SUCCESS;
}

enum StatusCode Properties::append(const uint8_t *key, uint16_t keySize,
				   const uint8_t *value, uint16_t valueSize)
{
	ByteArray *_key = new ByteArray(key, key + keySize);
	ByteArray *_value = new ByteArray(value, value + valueSize);

	return append(_key, _value);
}

enum StatusCode Properties::append(PropertyId id, ByteArray *src)
{
	if (!isAllowed(id)) {
		return StatusCode::INVALID_PROPERTY;
	}

	if (isEnabled(id)) {
		auto it = binProps.find((uint8_t)id);
		if (it == binProps.end()) {
			return StatusCode::INTERNAL_ERROR;
		}

		ByteArray *item = (*it).second;
		this->_wireSize += -item->size() + src->size();

		delete item;
		binProps.erase(it);
		binProps.insert(BinaryPropPair((uint8_t)id, src));
	} else {
		/* assume compiler will optimize this... */
		binProps.insert(BinaryPropPair((uint8_t)id, src));
		enableProperty(id);

		this->_wireSize += propertyIdSize + binaryLenSize + src->size();
	}

	return StatusCode::SUCCESS;
}

enum StatusCode Properties::append(PropertyId id, const uint8_t *data, uint16_t size)
{
	ByteArray *src = new ByteArray(data, data + size);

	return append(id, src);
}

enum StatusCode Properties::append(PropertyId id, uint32_t value, uint32_t wireSize)
{
	if (!isAllowed(id)) {
		return StatusCode::INVALID_PROPERTY;
	}

	if (isEnabled(id)) {
		auto it = numProps.find((uint8_t)id);
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

	return StatusCode::SUCCESS;
}

const ByteArray &Properties::valueBinary(PropertyId id) const
{
	if (!isEnabled(id)) {
		static auto none = ByteArray();

		return none;
	}

	auto it = binProps.find((uint8_t)id);

	return *((*it).second);
}

uint32_t Properties::valueNum(PropertyId id) const
{
	if (!isEnabled(id)) {
		return 0;
	}

	auto it = numProps.find((uint8_t)id);

	return ((*it).second).num;
}

void Properties::enableProperty(PropertyId id)
{
	this->enabledProperties |= pow2(id);
}

enum StatusCode Properties::payloadFormatIndicator(bool v)
{
	return append(PropertyId::PAYLOAD_FORMAT_INDICATOR, v, 1);
}

bool Properties::payloadFormatIndicator(void) const
{
	return valueNum(PropertyId::PAYLOAD_FORMAT_INDICATOR);
}

enum StatusCode Properties::publicationExpiryInterval(uint32_t v)
{
	return append(PropertyId::PUBLICATION_EXPIRY_INTERVAL, v, 4);
}

uint32_t Properties::publicationExpiryInterval(void) const
{
	return valueNum(PropertyId::PUBLICATION_EXPIRY_INTERVAL);
}

enum StatusCode Properties::contentType(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::CONTENT_TYPE, data, size);
}

enum StatusCode Properties::contentType(const char *str)
{
	return contentType((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::contentType(void) const
{
	return valueBinary(PropertyId::CONTENT_TYPE);
}

enum StatusCode Properties::responseTopic(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::RESPONSE_TOPIC, data, size);
}

enum StatusCode Properties::responseTopic(const char *str)
{
	return responseTopic((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::responseTopic(void) const
{
	return valueBinary(PropertyId::RESPONSE_TOPIC);
}

enum StatusCode Properties::subscriptionIdentifier(uint32_t v)
{
	auto ws = VBIWireSize(v);

	if (v == 0 || ws == 0) {
		return StatusCode::INVALID_VBI;
	}

	return append(PropertyId::SUBSCRIPTION_IDENTIFIER, v, ws);
}

uint32_t Properties::subscriptionIdentifier(void) const
{
	return valueNum(PropertyId::SUBSCRIPTION_IDENTIFIER);
}

enum StatusCode Properties::correlationData(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::CORRELATION_DATA, data, size);
}

const ByteArray &Properties::correlationData(void) const
{
	return valueBinary(PropertyId::CORRELATION_DATA);
}

enum StatusCode Properties::sessionExpiryInterval(uint32_t v)
{
	return append(PropertyId::SESSION_EXPIRY_INTERVAL, v, 4);
}

uint32_t Properties::sessionExpiryInterval(void) const
{
	return valueNum(PropertyId::SESSION_EXPIRY_INTERVAL);
}

enum StatusCode Properties::assignedClientIdentifier(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::ASSIGNED_CLIENT_IDENTIFIER, data, size);
}

enum StatusCode Properties::assignedClientIdentifier(const char *str)
{
	return assignedClientIdentifier((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::assignedClientIdentifier(void) const
{
	return valueBinary(PropertyId::ASSIGNED_CLIENT_IDENTIFIER);
}

enum StatusCode Properties::serverKeepAlive(uint16_t v)
{
	return append(PropertyId::SERVER_KEEP_ALIVE, v, 2);
}

uint16_t Properties::serverKeepAlive(void) const
{
	return valueNum(PropertyId::SERVER_KEEP_ALIVE);
}

enum StatusCode Properties::authenticationMethod(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::AUTH_METHOD, data, size);
}

enum StatusCode  Properties::authenticationMethod(const char *str)
{
	return authenticationMethod((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::authenticationMethod(void) const
{
	return valueBinary(PropertyId::AUTH_METHOD);
}

enum StatusCode Properties::authenticationData(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::AUTH_DATA, data, size);
}

const ByteArray &Properties::authenticationData(void) const
{
	return valueBinary(PropertyId::AUTH_DATA);
}

enum StatusCode Properties::requestProblemInformation(bool v)
{
	return append(PropertyId::REQUEST_PROBLEM_INFORMATION, v, 1);
}

bool Properties::requestProblemInformation(void) const
{
	return valueNum(PropertyId::REQUEST_PROBLEM_INFORMATION);
}

enum StatusCode Properties::willDelayInterval(uint32_t v)
{
	return append(PropertyId::WILL_DELAY_INTERVAL, v, 4);
}

uint32_t Properties::willDelayInterval(void) const
{
	return valueNum(PropertyId::WILL_DELAY_INTERVAL);
}

enum StatusCode Properties::requestResponseInformation(bool v)
{
	return append(PropertyId::REQUEST_RESPONSE_INFORMATION, v, 1);
}

bool Properties::requestResponseInformation(void) const
{
	return valueNum(PropertyId::REQUEST_RESPONSE_INFORMATION);
}

enum StatusCode Properties::responseInformation(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::RESPONSE_INFORMATION, data, size);
}

enum StatusCode Properties::responseInformation(const char *str)
{
	return responseInformation((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::responseInformation(void) const
{
	return valueBinary(PropertyId::RESPONSE_INFORMATION);
}

enum StatusCode Properties::serverReference(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::SERVER_REFERENCE, data, size);
}

enum StatusCode Properties::serverReference(const char *str)
{
	return serverReference((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::serverReference(void) const
{
	return valueBinary(PropertyId::SERVER_REFERENCE);
}

enum StatusCode Properties::reasonString(const uint8_t *data, uint16_t size)
{
	return append(PropertyId::REASON_STR, data, size);
}

enum StatusCode Properties::reasonString(const char *str)
{
	return reasonString((const uint8_t *)str, strlen(str));
}

const ByteArray &Properties::reasonString(void) const
{
	return valueBinary(PropertyId::REASON_STR);
}

enum StatusCode Properties::receiveMaximum(uint16_t v)
{
	return append(PropertyId::RECEIVE_MAXIMUM, v, 2);
}

uint16_t Properties::receiveMaximum(void) const
{
	return valueNum(PropertyId::RECEIVE_MAXIMUM);
}

enum StatusCode Properties::topicAliasMaximum(uint16_t v)
{
	return append(PropertyId::TOPIC_ALIAS_MAXIMUM, v, 2);
}

uint16_t Properties::topicAliasMaximum(void) const
{
	return valueNum(PropertyId::TOPIC_ALIAS_MAXIMUM);
}

enum StatusCode Properties::topicAlias(uint16_t v)
{
	return append(PropertyId::TOPIC_ALIAS, v, 2);
}

uint16_t Properties::topicAlias(void) const
{
	return valueNum(PropertyId::TOPIC_ALIAS);
}

enum StatusCode Properties::maximumQoS(enum PktQoS qos)
{
	if (validQoS(qos) == false) {
		return StatusCode::INVALID_QOS;
	}

	return append(PropertyId::MAXIMUM_QOS, (uint8_t)qos, 1);

	return StatusCode::SUCCESS;
}

enum PktQoS Properties::maximumQoS(void) const
{
	return (enum PktQoS)valueNum(PropertyId::MAXIMUM_QOS);
}

enum StatusCode Properties::retainAvailable(bool v)
{
	return append(PropertyId::RETAIN_AVAILABLE, v, 1);
}

bool Properties::retainAvailable(void) const
{
	return valueNum(PropertyId::RETAIN_AVAILABLE);
}

enum StatusCode Properties::userProperty(const uint8_t *key, uint16_t keySize,
			      const uint8_t *value, uint16_t valueSize)
{
	return append(key, keySize, value, valueSize);
}

enum StatusCode Properties::userProperty(const char *key, const char *val)
{
	return userProperty((const uint8_t *)key, strlen(key),
			    (const uint8_t *)val, strlen(val));
}

const UserProperty &Properties::userProperty(void) const
{
	return userProps;
}

enum StatusCode Properties::maximumPacketSize(uint32_t v)
{
	return append(PropertyId::MAXIMUM_PACKET_SIZE, v, 4);
}

uint32_t Properties::maximumPacketSize(void) const
{
	return valueNum(PropertyId::MAXIMUM_PACKET_SIZE);
}

enum StatusCode Properties::wildcardSubscriptionAvailable(bool v)
{
	return append(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE, v, 1);
}

bool Properties::wildcardSubscriptionAvailable(void) const
{
	return valueNum(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE);
}

enum StatusCode Properties::subscriptionIdentifierAvailable(bool v)
{
	return append(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE, v, 1);
}

bool Properties::subscriptionIdentifierAvailable(void) const
{
	return valueNum(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE);
}

enum StatusCode Properties::sharedSubscriptionAvailable(bool v)
{
	return append(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE, v, 1);
}

bool Properties::sharedSubscriptionAvailable(void) const
{
	return valueNum(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE);
}

enum StatusCode Properties::read(AppBuf &buf)
{
	enum StatusCode rc;
	uint32_t fieldLen;
	uint8_t propWSWS;
	uint32_t propWS;
	uint8_t numberWS;
	uint32_t number;
	ByteArray *value;
	ByteArray *key;

	rc = buf.readVBI(propWS, propWSWS);
	if (rc != StatusCode::SUCCESS) {
		return rc;
	}

	if (propWS > buf.bytesToRead()) {
		return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
	}

	const uint32_t bytesAtBeginning = buf.bytesToRead();
	while (bytesAtBeginning - buf.bytesToRead() < propWS) {
		auto id = buf.readNum8();

		switch ((enum PropertyId)id) {
		case PropertyId::PAYLOAD_FORMAT_INDICATOR:
		case PropertyId::REQUEST_PROBLEM_INFORMATION:
		case PropertyId::REQUEST_RESPONSE_INFORMATION:
		case PropertyId::MAXIMUM_QOS:
		case PropertyId::RETAIN_AVAILABLE:
		case PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE:
		case PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE:
		case PropertyId::SHARED_SUBSCRIPTION_AVAILABLE:
			if (buf.bytesToRead() < 1) {
				return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
			}

			number = buf.readNum8();
			fieldLen = 1;

			rc = this->append((PropertyId)id, number, fieldLen);
			if (rc != StatusCode::SUCCESS) {
				return rc;
			}
			break;

		case PropertyId::SERVER_KEEP_ALIVE:
		case PropertyId::RECEIVE_MAXIMUM:
		case PropertyId::TOPIC_ALIAS_MAXIMUM:
		case PropertyId::TOPIC_ALIAS:
			if (buf.bytesToRead() < 2) {
				return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
			}

			number = buf.readNum16();
			fieldLen = 2;

			rc = this->append((PropertyId)id, number, fieldLen);
			if (rc != StatusCode::SUCCESS) {
				return rc;
			}
			break;

		case PropertyId::PUBLICATION_EXPIRY_INTERVAL:
		case PropertyId::SESSION_EXPIRY_INTERVAL:
		case PropertyId::WILL_DELAY_INTERVAL:
		case PropertyId::MAXIMUM_PACKET_SIZE:
			if (buf.bytesToRead() < 4) {
				return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
			}

			number = buf.readNum32();
			fieldLen = 4;

			rc = this->append((PropertyId)id, number, fieldLen);
			if (rc != StatusCode::SUCCESS) {
				return rc;
			}
			break;

		case PropertyId::CONTENT_TYPE:
		case PropertyId::RESPONSE_TOPIC:
		case PropertyId::CORRELATION_DATA:
		case PropertyId::ASSIGNED_CLIENT_IDENTIFIER:
		case PropertyId::AUTH_METHOD:
		case PropertyId::AUTH_DATA:
		case PropertyId::RESPONSE_INFORMATION:
		case PropertyId::SERVER_REFERENCE:
		case PropertyId::REASON_STR:
			value = new ByteArray();
			rc = buf.readBinary(*value);
			if (rc != StatusCode::SUCCESS) {
				goto lb_cleanup;
			}

			rc = this->append((PropertyId)id, value);
			if (rc != StatusCode::SUCCESS) {
			lb_cleanup:
				delete value;
				return rc;
			}
			break;

		case PropertyId::SUBSCRIPTION_IDENTIFIER:
			rc = buf.readVBI(number, numberWS);
			if (rc != StatusCode::SUCCESS) {
				return rc;
			}

			rc = this->append((PropertyId)id, number, numberWS);
			if (rc != StatusCode::SUCCESS) {
				return rc;
			}
			break;

		case PropertyId::USER_PROPERTY:
			key = new ByteArray();
			value = new ByteArray();
			rc = buf.readKeyValue(*key, *value);
			if (rc != StatusCode::SUCCESS) {
				goto lb_cleanup_kv;
			}

			rc = this->append(key, value);
			if (rc != StatusCode::SUCCESS) {
			lb_cleanup_kv:
				delete key;
				delete value;

				return rc;
			}
			break;
		default:
			return StatusCode::INVALID_PROPERTY;
		}
	}

	/* bytes read is different to wire size */
	if (propWS != this->wireSize()) {
		return StatusCode::INVALID_REMLEN;
	}

	return StatusCode::SUCCESS;
}


static void writeNumProp(AppBuf &buf, uint32_t num, uint8_t size)
{
	typedef void (AppBuf::*WriteNumPtr)(uint32_t);
	WriteNumPtr ptrs[] = { &AppBuf::writeNum8, &AppBuf::writeNum16, &AppBuf::writeNum32 };

	WriteNumPtr ptr = ptrs[size / 2];

	(buf.*ptr)(num);
}

enum StatusCode Properties::write(AppBuf &buf)
{
	uint8_t propWSWS = VBIWireSize(this->wireSize());

	if (propWSWS == 0) {
		return StatusCode::SUCCESS;
	}

	if (propWSWS + this->wireSize() > buf.bytesToWrite()) {
		return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
	}

	buf.writeVBI(this->wireSize());

	auto itNum = numProps.begin();
	while (itNum != numProps.end()) {
		auto id = (*itNum).first;
		auto num = (*itNum).second;

		buf.writeNum8(id);
		if ((enum PropertyId)id != PropertyId::SUBSCRIPTION_IDENTIFIER) {
			writeNumProp(buf, num.num, num.size);
		} else {
			buf.writeVBI(num.num);
		}

		itNum++;
	}

	auto itBin = binProps.begin();
	while (itBin != binProps.end()) {
		auto id = (*itBin).first;
		auto item = (*itBin).second;

		buf.writeNum8(id);
		buf.writeBinary(item->data(), item->size());

		itBin++;
	}

	auto itUser = userProps.begin();
	while (itUser != userProps.end()) {
		auto key = (*itUser).first;
		auto value = (*itUser).second;

		buf.writeNum8((uint8_t)PropertyId::USER_PROPERTY);
		buf.writeBinary(key->data(), key->size());
		buf.writeBinary(value->data(), value->size());

		itUser++;
	}

	return StatusCode::SUCCESS;
}

}

