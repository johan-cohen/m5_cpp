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

#ifndef __PKT_CONNECT_HPP__
#define __PKT_CONNECT_HPP__

#include "Properties.hpp"
#include "Packet.hpp"
#include "Common.hpp"

#include <cstdint>

namespace m5 {

class PktConnect : public Packet {
private:
	uint16_t _keepAlive = 0;

	uint8_t connectFlags = 0x00;

	uint8_t _willRetain = 0;
	uint8_t _cleanStart = 1;

	ByteArray _clientId;
	ByteArray _willTopic;
	ByteArray _willMsg;
	ByteArray _userName;
	ByteArray _password;

	uint8_t packConnectFlags(void);
	uint32_t payloadWireSize(void) const;

	enum StatusCode writeVariableHeader(AppBuf &buf) override;
	enum StatusCode writePayload(AppBuf &buf) override;

	enum StatusCode readVariableHeader(AppBuf &buf) override;
	enum StatusCode readPayload(AppBuf &buf) override;

	void init(const uint8_t *clientId, uint16_t len, bool cleanStart);

	bool flagCleanStart(uint8_t flags);
	bool flagWillMsg(uint8_t flags);
	enum PktQoS flagWillQoS(uint8_t flags);
	bool flagWillRetain(uint8_t flags);
	bool flagPassword(uint8_t flags);
	bool flagUserName(uint8_t flags);

	bool validClientIdSize(uint16_t size);

public:
	PktConnect();
	PktConnect(const uint8_t *clientId, uint16_t len, bool cleanStart = true);
	PktConnect(const char *clientId, bool cleanStart = true);
	PktConnect(AppBuf &buf);
	~PktConnect();

	uint32_t writeTo(AppBuf &buf) override;
	uint32_t readFrom(AppBuf &buf) override;

	uint16_t keepAlive(void) const { return this->_keepAlive; }
	void keepAlive(uint16_t keepAlive);

	bool cleanStart(void) const { return this->_cleanStart; }
	void cleanStart(bool cleanStart);

	const ByteArray &clientId(void) const { return _clientId; }
	enum StatusCode clientId(const uint8_t *data, uint16_t size);
	enum StatusCode clientId(const char *str);

	const ByteArray &willTopic(void) const { return _willTopic; }
	const ByteArray &willMsg(void) const { return _willMsg; }

	enum StatusCode will(const uint8_t *topic, uint16_t topic_size,
			     const uint8_t *msg, uint16_t msg_size);
	enum StatusCode will(const char *topic, const char *msg);

	bool willRetain(void) const { return this->_willRetain; }
	void willRetain(bool willRetain);

	enum PktQoS willQoS(void) const { return Packet::QoS(); }
	enum StatusCode willQoS(enum PktQoS qos) { return Packet::QoS(qos); }

	const ByteArray &userName(void) const { return _userName; }
	void userName(const uint8_t *data, uint16_t size);
	void userName(const char *str);

	const ByteArray &password(void) const { return _password; }
	void password(const uint8_t *data, uint16_t size);
	void password(const char *str);

	enum StatusCode sessionExpiryInterval(uint32_t v);
	uint32_t sessionExpiryInterval(void) const;

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

	enum StatusCode receiveMaximum(uint16_t v);
	uint16_t receiveMaximum(void) const;

	enum StatusCode topicAliasMaximum(uint16_t v);
	uint16_t topicAliasMaximum(void) const;

	enum StatusCode userProperty(const uint8_t *key, uint16_t keySize,
				     const uint8_t *value, uint16_t valueSize);
	enum StatusCode userProperty(const char *key, const char *val);
	const UserProperty &userProperty(void) const;

	enum StatusCode maximumPacketSize(uint32_t v);
	uint32_t maximumPacketSize(void) const;
};

}

#endif

