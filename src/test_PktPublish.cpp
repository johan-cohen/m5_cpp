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

#include "PktPublish.hpp"
#include "test_Common.hpp"

#include <cstring>

int test(void)
{
	m5::PktPublish *publish;
	const char str[] = "Hello, World!";
	const m5::ByteArray array(str, str + strlen(str));
	const uint8_t *payload = (const uint8_t *)str;
	const uint16_t payloadSize = strlen(str);
	uint16_t u16 = 0xABCD;
	m5::AppBuf buf(128);

	publish = new m5::PktPublish();

	publish->retain(true);
	if (publish->retain() != true) {
		throw std::logic_error("Retain");
	}

	publish->QoS(m5::PktQoS::QoS2);
	if (publish->QoS() != m5::PktQoS::QoS2) {
		throw std::logic_error("QoS");
	}

	publish->dup(true);
	if (publish->dup() != true) {
		throw std::logic_error("DUP");
	}

	publish->topic(str);
	if (publish->topic() != array) {
		throw std::logic_error("Topic");
	}

	publish->packetId(u16);
	if (publish->packetId() != u16) {
		throw std::logic_error("Packet Id");
	}

	publish->payload(payload, payloadSize);
	if (publish->payload() != array) {
		throw std::logic_error("Payload");
	}

	publish->writeTo(buf);

	m5::PktPublish publishRead;
	publishRead.readFrom(buf);

	if (publish->retain() != publishRead.retain()) {
		throw std::logic_error("read: Retain");
	}
	if (publish->QoS() != publishRead.QoS()) {
		throw std::logic_error("read: QoS");
	}
	if (publish->dup() != publishRead.dup()) {
		throw std::logic_error("read: DUP");
	}
	if (publish->topic() != publishRead.topic()) {
		throw std::logic_error("read: Topic");
	}
	if (publish->packetId() != publishRead.packetId()) {
		throw std::logic_error("read: Packet Id");
	}
	if (publish->payload() != publishRead.payload()) {
		throw std::logic_error("read: Payload");
	}

	delete publish;

	return 0;
}

int main(void)
{
	int rc;

	rc = test();
	test_rc(rc, "PktPublish");

	return 0;
}

