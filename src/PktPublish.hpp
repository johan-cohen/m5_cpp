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

#ifndef __PKT_PUBLISH_HPP__
#define __PKT_PUBLISH_HPP__

#include "Properties.hpp"
#include "Packet.hpp"
#include "Common.hpp"

namespace m5 {

class PktPublish : public Packet
{
private:
	PktQoS _QoS = PktQoS::QoS0;
	bool _retain = false;
	bool _dup = false;

	uint16_t _packetId = 0;
	ByteArray _topic;

	ByteArray _payload;

	Properties properties;

	void headerFlags(uint8_t firstByte);
	uint8_t headerFlags(void);

public:
	PktPublish();
	~PktPublish();

	PktQoS QoS(void) const { return this->_QoS; }
	void QoS(PktQoS q) { this->_QoS = q; }

	bool retain(void) const { return this->_retain; }
	void retain (bool f) { this->_retain = f; }

	bool dup(void) const { return this->_dup; }
	void dup(bool f) { this->_dup = f; }

	void packetId(uint16_t id) { this->_packetId = id; }
	uint16_t packetId(void) const { return _packetId; }

	void topic(const uint8_t *data, uint16_t size);
	void topic(const char *str);
	const ByteArray &topic(void) const { return _topic; }

	void payload(const uint8_t *data, uint16_t size);
	const ByteArray &payload(void) const { return _payload; }

	void payloadFormatIndicator(bool v);
	bool payloadFormatIndicator(void) const;

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

	void topicAlias(uint16_t v);
	uint16_t topicAlias(void) const;

	void userProperty(const uint8_t *key, uint16_t keySize,
			  const uint8_t *value, uint16_t valueSize);
	void userProperty(const char *key, const char *val);
	const UserProperty &userProperty(void) const;

	uint32_t writeTo(AppBuf &buf);
	uint32_t readFrom(AppBuf &buf);
	uint32_t getId(void) const { return (uint32_t)PktType::PUBLISH; }
};

}

#endif

