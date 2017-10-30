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

void PropertiesList::append(const uint8_t *key, uint16_t key_size,
			    const uint8_t *value, uint16_t value_size)
{
	if (!isAllowed(PropertyId::USER_PROPERTY)) {
		return;
	}

	std::vector<uint8_t> _key(key, key + key_size);
	std::vector<uint8_t> _val(value, value + value_size);

	userProps.push_back(KeyValuePair(_key, _val));

	this->enabledProperties |= __POW2(PropertyId::USER_PROPERTY);
}

void PropertiesList::append(PropertyId id, const uint8_t *data, uint16_t size)
{
	if (!isAllowed(id)) {
		return;
	}

	if (isEnabled(id)) {
		auto it = binProps.find(id);
		if (it == binProps.end()) {
			throw std::out_of_range("Property enabled but not found");
		}

		std::vector<uint8_t> &item = (*it).second;
		item.assign(data, data + size);

	} else {
		binProps.insert(
			BinaryPropPair(id, std::vector<uint8_t>(data, data + size)));
		this->enabledProperties |= __POW2(id);
	}
}

void PropertiesList::append(PropertyId id, uint32_t value)
{
	if (!isAllowed(id)) {
		return;
	}

	if (isEnabled(id)) {
		auto it = numProps.find(id);
		(*it).second = value;
	} else {
		numProps.insert(NumPropPair((uint8_t)id, value));
	}

	this->enabledProperties |= __POW2(id);
}

const std::vector<uint8_t> &PropertiesList::valueBinary(PropertyId id)
{
	if (!isEnabled(id)) {
		static auto none = std::vector<uint8_t>();

		return none;
	}

	auto it = binProps.find(id);

	return (*it).second;
}

uint32_t PropertiesList::valueNum(PropertyId id)
{
	if (!isEnabled(id)) {
		return 0;
	}

	auto it = numProps.find(id);

	return (*it).second;
}

void PropertiesList::payloadFormatIndicator(uint8_t v)
{
	append(PropertyId::PAYLOAD_FORMAT_INDICATOR, v);
}

uint8_t PropertiesList::payloadFormatIndicator(void)
{
	return valueNum(PropertyId::PAYLOAD_FORMAT_INDICATOR);
}

void PropertiesList::publicationExpiryInterval(uint32_t v)
{
	append(PropertyId::PUBLICATION_EXPIRY_INTERVAL, v);
}

uint32_t PropertiesList::publicationExpiryInterval(void)
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

const std::vector<uint8_t> &PropertiesList::contentType(void)
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

const std::vector<uint8_t> &PropertiesList::responseTopic(void)
{
	return valueBinary(PropertyId::RESPONSE_TOPIC);
}

void PropertiesList::subscriptionIdentifier(uint32_t v)
{
	append(PropertyId::SUBSCRIPTION_IDENTIFIER, v);
}

uint32_t PropertiesList::subscriptionIdentifier(void)
{
	return valueNum(PropertyId::SUBSCRIPTION_IDENTIFIER);
}

void PropertiesList::correlationData(const uint8_t *data, uint16_t size)
{
	append(PropertyId::CORRELATION_DATA, data, size);
}

const std::vector<uint8_t> &PropertiesList::correlationData(void)
{
	return valueBinary(PropertyId::CORRELATION_DATA);
}

void PropertiesList::sessionExpiryInterval(uint32_t v)
{
	append(PropertyId::SESSION_EXPIRY_INTERVAL, v);
}

uint32_t PropertiesList::sessionExpiryInterval(void)
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

const std::vector<uint8_t> &PropertiesList::assignedClientIdentifier(void)
{
	return valueBinary(PropertyId::ASSIGNED_CLIENT_IDENTIFIER);
}

void PropertiesList::serverKeepAlive(uint16_t v)
{
	append(PropertyId::SERVER_KEEP_ALIVE, v);
}

uint16_t PropertiesList::serverKeepAlive(void)
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

const std::vector<uint8_t> &PropertiesList::authenticationMethod(void)
{
	return valueBinary(PropertyId::AUTH_METHOD);
}

void PropertiesList::authenticationData(const uint8_t *data, uint16_t size)
{
	append(PropertyId::AUTH_DATA, data, size);
}

const std::vector<uint8_t> &PropertiesList::authenticationData(void)
{
	return valueBinary(PropertyId::AUTH_DATA);
}

void PropertiesList::requestProblemInformation(bool v)
{
	append(PropertyId::REQUEST_PROBLEM_INFORMATION, v);
}

bool PropertiesList::requestProblemInformation(void)
{
	return valueNum(PropertyId::REQUEST_PROBLEM_INFORMATION);
}

void PropertiesList::willDelayInterval(uint32_t v)
{
	append(PropertyId::WILL_DELAY_INTERVAL, v);
}

uint32_t PropertiesList::willDelayInterval(void)
{
	return valueNum(PropertyId::WILL_DELAY_INTERVAL);
}

void PropertiesList::requestResponseInformation(bool v)
{
	append(PropertyId::REQUEST_RESPONSE_INFORMATION, v);
}

bool PropertiesList::requestResponseInformation(void)
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

const std::vector<uint8_t> &PropertiesList::responseInformation(void)
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

const std::vector<uint8_t> &PropertiesList::serverReference(void)
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

const std::vector<uint8_t> &PropertiesList::reasonString(void)
{
	return valueBinary(PropertyId::REASON_STR);
}

void PropertiesList::receiveMaximum(uint16_t v)
{
	append(PropertyId::RECEIVE_MAXIMUM, v);
}

uint16_t PropertiesList::receiveMaximum(void)
{
	return valueNum(PropertyId::RECEIVE_MAXIMUM);
}

void PropertiesList::topicAliasMaximum(uint16_t v)
{
	append(PropertyId::TOPIC_ALIAS_MAXIMUM, v);
}

uint16_t PropertiesList::topicAliasMaximum(void)
{
	return valueNum(PropertyId::TOPIC_ALIAS_MAXIMUM);
}

void PropertiesList::topicAlias(uint16_t v)
{
	append(PropertyId::TOPIC_ALIAS, v);
}

uint16_t PropertiesList::topicAlias(void)
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

	append(PropertyId::MAXIMUM_QOS, (uint8_t)qos);
}

PktQoS PropertiesList::maximumQoS(void)
{
	return (PktQoS)valueNum(PropertyId::MAXIMUM_QOS);
}

void PropertiesList::retainAvailable(bool v)
{
	append(PropertyId::RETAIN_AVAILABLE, v);
}

bool PropertiesList::retainAvailable(void)
{
	return valueNum(PropertyId::RETAIN_AVAILABLE);
}

void PropertiesList::userProperty(const uint8_t *key, uint16_t key_len,
				  const uint8_t *value, uint16_t value_len)
{
	append(key, key_len, value, value_len);
}

void PropertiesList::userProperty(const char *key, const char *val)
{
	userProperty((const uint8_t *)key, strlen(key), (const uint8_t *)val, strlen(val));
}

const UserProperty &PropertiesList::userProperty(void)
{
	return userProps;
}

void PropertiesList::maximumPacketSize(uint32_t v)
{
	append(PropertyId::MAXIMUM_PACKET_SIZE, v);
}

uint32_t PropertiesList::maximumPacketSize(void)
{
	return valueNum(PropertyId::MAXIMUM_PACKET_SIZE);
}

void PropertiesList::wildcardSubscriptionAvailable(bool v)
{
	append(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE, v);
}

bool PropertiesList::wildcardSubscriptionAvailable(void)
{
	return valueNum(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE);
}

void PropertiesList::subscriptionIdentifierAvailable(bool v)
{
	append(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE, v);
}

bool PropertiesList::subscriptionIdentifierAvailable(void)
{
	return valueNum(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE);
}

void PropertiesList::sharedSubscriptionAvailable(bool v)
{
	append(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE, v);
}

bool PropertiesList::sharedSubscriptionAvailable(void)
{
	return valueNum(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE);
}

}
