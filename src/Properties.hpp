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

#ifndef __PROPERTIES_HPP__
#define __PROPERTIES_HPP__

#include "PropertyNode.hpp"
#include "Common.hpp"

#include <map>

namespace m5 {

enum PropertyId {
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

class PropertiesList {
private:
	std::multimap<uint8_t, PropertyNode *> propList;
	enum PktType pktType = PktType::RESERVED;
	uint64_t enabledProperties = 0;
	uint64_t properties = 0;

	void push(PropertyNode *node);
	void deleteList();

	void computePktFlags(void);

public:
	PropertiesList(const PktType type = PktType::RESERVED);
	~PropertiesList();

	enum PktType packetType() const { return pktType; }
	void resetPacketType(const enum PktType type);

	uint64_t allowed() const { return properties; }
	uint64_t enabled() const { return enabledProperties; }

	bool isAllowed(PropertyId id) const;
	bool isEnabled(PropertyId id) const;

	template <typename T> void addNum(PropertyId id, T v);
	void add(PropertyId id, uint8_t *data, uint16_t size);

	template <typename T> T valueNum(PropertyId id);
	uint8_t *value(PropertyId id, uint16_t &size);

	void payloadFormatIndicator(uint8_t v);
	uint8_t payloadFormatIndicator(void);

	void publicationExpiryInterval(uint32_t v);
	uint32_t publicationExpiryInterval(void);

	void sessionExpiryInterval(uint32_t v);
	uint32_t sessionExpiryInterval(void);

	void serverKeepAlive(uint16_t v);
	uint16_t serverKeepAlive(void);

	void requestProblemInformation(bool v);
	bool requestProblemInformation(void);

	void willDelayInterval(uint32_t v);
	uint32_t willDelayInterval(void);

	void requestResponseInformation(bool v);
	bool requestResponseInformation(void);

	void receiveMaximum(uint16_t v);
	uint16_t receiveMaximum(void);

	void topicAliasMaximum(uint16_t v);
	uint16_t topicAliasMaximum(void);

	void topicAlias(uint16_t v);
	uint16_t topicAlias(void);
};

}

#endif
