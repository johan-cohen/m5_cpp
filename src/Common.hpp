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


#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <cstdint>
#include <vector>

namespace m5 {

typedef std::vector<uint8_t> ByteArray;

static const uint32_t propertyIdSize = 1;
static const uint32_t binaryLenSize = 2;
static const uint32_t stringLenSize = binaryLenSize;
static const uint32_t integerLenSize = 2;

static const uint32_t fixedHeaderSize = 1;
static const uint32_t remLenMinSize = 1;
static const uint32_t topicNameMinSize = 1;
static const uint32_t packetIdSize = 2;
static const uint32_t propertyMinSize = 1; /* empty property field, just length */
static const uint32_t reasonCodeSize = 1;
static const uint32_t subscribeOptionsSize = 1;
static const uint32_t flagsSize = 1;

enum class PktQoS {
	QoS0 = 0,
	QoS1,
	QoS2
};

enum class PktType {
	RESERVED = 0,
	CONNECT,
	CONNACK,
	PUBLISH,
	PUBACK,
	PUBREC,
	PUBREL,
	PUBCOMP,
	SUBSCRIBE,
	SUBACK,
	UNSUBSCRIBE,
	UNSUBACK,
	PINGREQ,
	PINGRESP,
	DISCONNECT,
	AUTH
};

enum class PropertyId {
	PAYLOAD_FORMAT_INDICATOR = 0x01,
	PUBLICATION_EXPIRY_INTERVAL,
	CONTENT_TYPE,
	/* empty */
	/* empty */
	/* empty */
	/* empty */
	RESPONSE_TOPIC = 0x08,
	CORRELATION_DATA,
	/* empty */
	SUBSCRIPTION_IDENTIFIER = 0x0B,
	/* empty */
	/* empty */
	/* empty */
	/* empty */
	/* empty */
	SESSION_EXPIRY_INTERVAL = 0x11,
	ASSIGNED_CLIENT_IDENTIFIER,
	SERVER_KEEP_ALIVE,
	/* empty */
	AUTH_METHOD = 0x15,
	AUTH_DATA,
	REQUEST_PROBLEM_INFORMATION,
	WILL_DELAY_INTERVAL,
	REQUEST_RESPONSE_INFORMATION,
	RESPONSE_INFORMATION,
	/* empty */
	SERVER_REFERENCE  = 0x1C,
	/* empty */
	/* empty */
	REASON_STR = 0x1F,
	/* empty */
	RECEIVE_MAXIMUM = 0x21,
	TOPIC_ALIAS_MAXIMUM,
	TOPIC_ALIAS,
	MAXIMUM_QOS,
	RETAIN_AVAILABLE,
	USER_PROPERTY,
	MAXIMUM_PACKET_SIZE,
	WILDCARD_SUBSCRIPTION_AVAILABLE,
	SUBSCRIPTION_IDENTIFIER_AVAILABLE,
	SHARED_SUBSCRIPTION_AVAILABLE,
};

enum class ReasonCode {
	SUCCESS = 0,
	NORMAL_DISCONNECTION = 0,
	GRANTED_QOS0 = 0,
	GRANTED_QOS1,
	GRANTED_QOS2,
	DISCONNECT_WILL = 4,
	NO_MATCHING_SUBSCRIBERS = 16,
	NO_SUBSCRIPTION_EXISTED,
	CONTINUE_AUTHENTICATION = 24,
	RE_AUTHENTICATE,
	UNSPECIFIED_ERROR = 128,
	MALFORMED_PACKET,
	PROTOCOL_ERROR,
	IMPLEMENTATION_SPECIFIC_ERROR,
	UNSUPPORTED_PROTOCOL_VERSION,
	CLIENT_IDENTIFIER_NOT_VALID,
	BAD_USER_NAME_OR_PASSWORD,
	NOT_AUTHORIZED,
	SERVER_UNAVAILABLE,
	SERVER_BUSY,
	BANNED,
	SERVER_SHUTTING_DOWN,
	BAD_AUTHENTICATION_METHOD,
	KEEP_ALIVE_TIMEOUT,
	SESSION_TAKEN_OVER,
	TOPIC_FILTER_INVALID,
	TOPIC_NAME_INVALID,
	PACKET_IDENTIFIER_IN_USE,
	PACKET_IDENTIFIER_NOT_FOUND,
	RECEIVE_MAXIMUM_EXCEEDED,
	TOPIC_ALIAS_INVALID,
	PACKET_TOO_LARGE,
	MESSAGE_RATE_TOO_HIGH,
	QUOTA_EXCEEDED,
	ADMINISTRATIVE_ACTION,
	PAYLOAD_FORMAT_INVALID,
	RETAIN_NOT_SUPPORTED,
	QOS_NOT_SUPPORTED,
	USE_ANOTHER_SERVER,
	SERVER_MOVED,
	SHARED_SUBSCRIPTION_NOT_SUPPORTED,
	CONNECTION_RATE_EXCEEDED,
	MAXIMUM_CONNECT_TIME,
	SUBSCRIPTION_IDENTIFIERS_NOT_SUPPORTED,
	WILDCARD_SUBSCRIPTION_NOT_SUPPORTED
};

enum class StatusCode {
	SUCCESS = 0,
	INVALID_FIXED_HEADER,
	INVALID_PACKET_TYPE,
	INVALID_VBI,
	INVALID_PROPERTY_VBI,
	INVALID_REMLEN,
	INVALID_REMLEN_VBI,
	NOT_ENOUGH_SPACE_IN_BUFFER,
	RESERVED_MUST_BE_ZERO,
	FINISHED_READING_INVALID_LENGTH,
	FINISHED_WRITING_INVALID_LENGTH,
	PROPERTY_WRITE_ERROR,
	INVALID_PROTOCOL_NAME,
	INVALID_PROTOCOL_VERSION,
	INVALID_CONNECT_PAYLOAD,
	INVALID_QOS,
	INVALID_TOPIC_NAME,
	INVALID_PACKET_ID,
	INVALID_ARGUMENT,
	INVALID_PROPERTY,
	INTERNAL_ERROR,
};

static inline uint8_t VBIWireSize(uint32_t value)
{
	if (value <= 127) {
		return 1;
	}
	if (value <= 16383) {
		return 2;
	}
	if (value <= 2097151) {
		return 3;
	}
	if (value <= 268435455) {
		return 4;
	}

	return 0;
}

static inline enum PktType packetType(uint8_t byte)
{
	return (enum PktType)((byte & 0xF0) >> 4);
}

static inline uint8_t firstByte(enum PktType type, uint8_t reserved = 0)
{
	return (((uint8_t)type) << 4) | (reserved & 0x0F);
}

static inline bool validQoS(enum PktQoS qos)
{
	switch (qos) {
	case PktQoS::QoS0:
	case PktQoS::QoS1:
	case PktQoS::QoS2:
		return true;
	default:
		return false;
	}
}

static inline bool validQoS(uint8_t qos)
{
	return validQoS((enum PktQoS)qos);
}

static inline bool validPacketId(uint16_t packetId)
{
	return packetId > 0;
}

}

#endif

