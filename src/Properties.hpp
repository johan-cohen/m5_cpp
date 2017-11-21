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

typedef std::pair<ByteArray *, ByteArray *> KeyValuePair;
typedef std::list<KeyValuePair> UserProperty;

class Properties {
private:
	struct NumSize { uint32_t num; uint8_t size; };
	typedef std::pair<uint8_t, ByteArray *> BinaryPropPair;
	typedef std::pair<uint8_t, NumSize> NumPropPair;

	std::map<uint8_t, ByteArray *> binProps;
	std::map<uint8_t, NumSize> numProps;
	UserProperty userProps;

	enum PktType pktType = PktType::RESERVED;
	uint64_t enabledProperties = 0;
	uint64_t allowedProperties = 0;
	uint32_t _wireSize = 0;

	void computePktFlags(void);

	enum StatusCode append(PropertyId id,
			       const uint8_t *data, uint16_t size);
	enum StatusCode append(PropertyId id, ByteArray *src);

	enum StatusCode append(PropertyId id, uint32_t v, uint32_t wireSize);

	enum StatusCode append(ByteArray *key, ByteArray *value);
	enum StatusCode append(const uint8_t *key, uint16_t keySize,
			       const uint8_t *value, uint16_t valueSize);

	const ByteArray &valueBinary(PropertyId id) const;
	uint32_t valueNum(PropertyId id) const;

	void resetPacketType(const enum PktType type);

	uint64_t allowed() const { return allowedProperties; }
	uint64_t enabled() const { return enabledProperties; }

	void enableProperty(PropertyId id);

public:
	Properties(const PktType type = PktType::RESERVED);
	~Properties();

	enum PktType packetType() const { return pktType; }

	bool isAllowed(PropertyId id) const;

	bool isEnabled(PropertyId id) const;

	enum StatusCode payloadFormatIndicator(bool v);
	bool payloadFormatIndicator(void) const;

	enum StatusCode publicationExpiryInterval(uint32_t v);
	uint32_t publicationExpiryInterval(void) const;

	enum StatusCode contentType(const uint8_t *data, uint16_t size);
	enum StatusCode contentType(const char *str);
	const ByteArray &contentType(void) const;

	enum StatusCode responseTopic(const uint8_t *data, uint16_t size);
	enum StatusCode responseTopic(const char *str);
	const ByteArray &responseTopic(void) const;

	enum StatusCode subscriptionIdentifier(uint32_t v);
	uint32_t subscriptionIdentifier(void) const;

	enum StatusCode correlationData(const uint8_t *data, uint16_t size);
	const ByteArray &correlationData(void) const;

	enum StatusCode sessionExpiryInterval(uint32_t v);
	uint32_t sessionExpiryInterval(void) const;

	enum StatusCode assignedClientIdentifier(const uint8_t *data, uint16_t size);
	enum StatusCode assignedClientIdentifier(const char *str);
	const ByteArray &assignedClientIdentifier(void) const;

	enum StatusCode serverKeepAlive(uint16_t v);
	uint16_t serverKeepAlive(void) const;

	enum StatusCode authenticationMethod(const uint8_t *data, uint16_t size);
	enum StatusCode authenticationMethod(const char *str);
	const ByteArray &authenticationMethod(void) const;

	enum StatusCode authenticationData(const uint8_t *data, uint16_t size);
	const ByteArray &authenticationData(void) const;

	enum StatusCode requestProblemInformation(bool v);
	bool requestProblemInformation(void) const;

	enum StatusCode willDelayInterval(uint32_t v);
	uint32_t willDelayInterval(void) const;

	enum StatusCode requestResponseInformation(bool v);
	bool requestResponseInformation(void) const;

	enum StatusCode responseInformation(const uint8_t *data, uint16_t size);
	enum StatusCode responseInformation(const char *str);
	const ByteArray &responseInformation(void) const;

	enum StatusCode serverReference(const uint8_t *data, uint16_t size);
	enum StatusCode serverReference(const char *str);
	const ByteArray &serverReference(void) const;

	enum StatusCode reasonString(const uint8_t *data, uint16_t size);
	enum StatusCode reasonString(const char *str);
	const ByteArray &reasonString(void) const;

	enum StatusCode receiveMaximum(uint16_t v);
	uint16_t receiveMaximum(void) const;

	enum StatusCode topicAliasMaximum(uint16_t v);
	uint16_t topicAliasMaximum(void) const;

	enum StatusCode topicAlias(uint16_t v);
	uint16_t topicAlias(void) const;

	enum StatusCode maximumQoS(enum PktQoS qos);
	enum PktQoS maximumQoS(void) const;

	enum StatusCode retainAvailable(bool v);
	bool retainAvailable(void) const;

	enum StatusCode userProperty(const uint8_t *key, uint16_t keySize,
				     const uint8_t *value, uint16_t valueSize);
	enum StatusCode userProperty(const char *key, const char *val);
	const UserProperty &userProperty(void) const;

	enum StatusCode maximumPacketSize(uint32_t v);
	uint32_t maximumPacketSize(void) const;

	enum StatusCode wildcardSubscriptionAvailable(bool v);
	bool wildcardSubscriptionAvailable(void) const;

	enum StatusCode subscriptionIdentifierAvailable(bool v);
	bool subscriptionIdentifierAvailable(void) const;

	enum StatusCode sharedSubscriptionAvailable(bool v);
	bool sharedSubscriptionAvailable(void) const;

	uint32_t wireSize(void) const { return _wireSize; }

	enum StatusCode read(AppBuf &buf);
	enum StatusCode write(AppBuf &buf);
};

}

#endif

