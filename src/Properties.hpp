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

#include "Common.hpp"
#include "AppBuf.hpp"

#include <list>
#include <map>

namespace m5 {

typedef std::pair<ByteArray, ByteArray > KeyValuePair;
typedef std::list<KeyValuePair> UserProperty;

class PropertiesList {
private:
	struct NumSize { uint32_t num; uint8_t size; };
	typedef std::pair<uint8_t, ByteArray> BinaryPropPair;
	typedef std::pair<uint8_t, NumSize> NumPropPair;

	std::map<uint8_t, ByteArray> binProps;
	std::map<uint8_t, NumSize> numProps;
	UserProperty userProps;

	enum PktType pktType = PktType::RESERVED;
	uint64_t enabledProperties = 0;
	uint64_t properties = 0;
	uint32_t _wireSize = 0;

	void computePktFlags(void);

	void append(PropertyId id, const uint8_t *data, uint16_t size);
	void append(PropertyId id, const ByteArray &src);

	void append(PropertyId id, uint32_t v, uint32_t wireSize);

	void append(const ByteArray &key, const ByteArray &value);
	void append(const uint8_t *key, uint16_t keySize,
		    const uint8_t *value, uint16_t valueSize);

	const ByteArray &valueBinary(PropertyId id) const;
	uint32_t valueNum(PropertyId id) const;

	void resetPacketType(const enum PktType type);

	uint64_t allowed() const { return properties; }
	uint64_t enabled() const { return enabledProperties; }

	void enableProperty(PropertyId id);

public:
	PropertiesList(const PktType type = PktType::RESERVED);
	~PropertiesList();

	enum PktType packetType() const { return pktType; }

	bool isAllowed(PropertyId id) const;
	bool isAllowed(uint8_t id) const;

	bool isEnabled(PropertyId id) const;

	void payloadFormatIndicator(uint8_t v);
	uint8_t payloadFormatIndicator(void) const;

	void publicationExpiryInterval(uint32_t v);
	uint32_t publicationExpiryInterval(void) const;

	void contentType(const uint8_t *data, uint16_t size);
	void contentType(const char *str);
	const ByteArray &contentType(void) const;

	void responseTopic(const uint8_t *data, uint16_t size);
	void responseTopic(const char *str);
	const ByteArray &responseTopic(void) const;

	void subscriptionIdentifier(uint32_t v);
	uint32_t subscriptionIdentifier(void) const;

	void correlationData(const uint8_t *data, uint16_t size);
	const ByteArray &correlationData(void) const;

	void sessionExpiryInterval(uint32_t v);
	uint32_t sessionExpiryInterval(void) const;

	void assignedClientIdentifier(const uint8_t *data, uint16_t size);
	void assignedClientIdentifier(const char *str);
	const ByteArray &assignedClientIdentifier(void) const;

	void serverKeepAlive(uint16_t v);
	uint16_t serverKeepAlive(void) const;

	void authenticationMethod(const uint8_t *data, uint16_t size);
	void authenticationMethod(const char *str);
	const ByteArray &authenticationMethod(void) const;

	void authenticationData(const uint8_t *data, uint16_t size);
	const ByteArray &authenticationData(void) const;

	void requestProblemInformation(bool v);
	bool requestProblemInformation(void) const;

	void willDelayInterval(uint32_t v);
	uint32_t willDelayInterval(void) const;

	void requestResponseInformation(bool v);
	bool requestResponseInformation(void) const;

	void responseInformation(const uint8_t *data, uint16_t size);
	void responseInformation(const char *str);
	const ByteArray &responseInformation(void) const;

	void serverReference(const uint8_t *data, uint16_t size);
	void serverReference(const char *str);
	const ByteArray &serverReference(void) const;

	void reasonString(const uint8_t *data, uint16_t size);
	void reasonString(const char *str);
	const ByteArray &reasonString(void) const;

	void receiveMaximum(uint16_t v);
	uint16_t receiveMaximum(void) const;

	void topicAliasMaximum(uint16_t v);
	uint16_t topicAliasMaximum(void) const;

	void topicAlias(uint16_t v);
	uint16_t topicAlias(void) const;

	void maximumQoS(PktQoS qos);
	PktQoS maximumQoS(void) const;

	void retainAvailable(bool v);
	bool retainAvailable(void) const;

	void userProperty(const uint8_t *key, uint16_t keySize,
			  const uint8_t *value, uint16_t valueSize);
	void userProperty(const char *key, const char *val);
	const UserProperty &userProperty(void) const;

	void maximumPacketSize(uint32_t v);
	uint32_t maximumPacketSize(void) const;

	void wildcardSubscriptionAvailable(bool v);
	bool wildcardSubscriptionAvailable(void) const;

	void subscriptionIdentifierAvailable(bool v);
	bool subscriptionIdentifierAvailable(void) const;

	void sharedSubscriptionAvailable(bool v);
	bool sharedSubscriptionAvailable(void) const;

	uint32_t wireSize(void) const { return _wireSize; }

	uint32_t read(AppBuf &buf);
	uint32_t write(AppBuf &buf);
};

}

#endif

