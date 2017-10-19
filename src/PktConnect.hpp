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

#include "ProtoEntity.hpp"
#include "Common.hpp"

#include <cstdint>

namespace m5 {

class PktConnect : public ProtoEntity {
private:
	uint16_t keepAlive = 0;

	uint8_t willRetain = 0;
	uint8_t willQoS = 0;
	uint8_t cleanStart = 1;

	uint8_t packConnectFlags(void);
	uint32_t payloadWireSize(void) const;
	uint32_t propertiesWireSize(void) const;

	void writePayload(AppBuf &buf);
	void writeProperties(AppBuf &buf, uint32_t propWireSize);

	void init(const uint8_t *clientId, uint16_t len, bool cleanStart);

	bool flagCleanStart(uint8_t flags);
	bool flagWillMsg(uint8_t flags);
	uint8_t flagWillQoS(uint8_t flags);
	bool flagWillRetain(uint8_t flags);
	bool flagPassword(uint8_t flags);
	bool flagUserName(uint8_t flags);

public:
	PktConnect() {};
	PktConnect(AppBuf &buf);
	PktConnect(const uint8_t *clientId, uint16_t len, bool cleanStart = true);
	PktConnect(const char *clientId, bool cleanStart = true);
	~PktConnect();

	AppBuf *clientId = nullptr;
	AppBuf *willTopic = nullptr;
	AppBuf *willMsg = nullptr;
	AppBuf *userName = nullptr;
	AppBuf *password = nullptr;

	uint16_t getKeepAlive(void) const { return this->keepAlive; }

	bool getWillRetain(void) const { return this->willRetain; }

	PktQoS getWillQoS(void) const { return (PktQoS)this->willQoS; }

	bool getCleanStart(void) const { return this->cleanStart; }

	uint32_t writeTo(AppBuf &buf) override;
	uint32_t readFrom(AppBuf &buf) override;
	uint32_t getId(void) const override;

	void setClientId(const uint8_t *data, uint16_t size);
	void setClientId(const char *str);

	void setWill(const uint8_t *topic, uint16_t topic_size,
		     const uint8_t *msg, uint16_t msg_size);
	void setWill(const char *topic, const char *msg);

	void setUserName(const uint8_t *data, uint16_t size);
	void setUserName(const char *str);

	void setPassword(const uint8_t *data, uint16_t size);
	void setPassword(const char *str);

	void setKeyAlive(uint16_t keepAlive);

	void setWillRetain(bool willRetain);

	void setWillQoS(enum PktQoS qos);

	void setCleanStart(bool cleanStart);
};

}
