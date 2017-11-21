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
	m5::StatusCode sc;

	publish = new m5::PktPublish();

	publish->retain(true);
	if (publish->retain() != true) {
		error_exit("Retain");
	}

	publish->QoS(m5::PktQoS::QoS2);
	if (publish->QoS() != m5::PktQoS::QoS2) {
		error_exit("QoS");
	}

	publish->dup(true);
	if (publish->dup() != true) {
		error_exit("DUP");
	}

	publish->topic(str);
	if (publish->topic() != array) {
		error_exit("Topic");
	}

	publish->packetId(u16);
	if (publish->packetId() != u16) {
		error_exit("Packet Id");
	}

	sc = publish->payload(payload, payloadSize);
	if (sc != m5::StatusCode::SUCCESS) {
		error_exit("Payload");
	}

	if (publish->payload() != array) {
		error_exit("Payload");
	}

	publish->writeTo(buf);
	if (publish->status() != m5::StatusCode::SUCCESS) {
		error_exit("write");
	}

	m5::PktPublish publishRead(buf);
	if (publishRead.status() != m5::StatusCode::SUCCESS) {
		error_exit("readFrom");
	}

	if (publish->retain() != publishRead.retain()) {
		error_exit("read: Retain");
	}
	if (publish->QoS() != publishRead.QoS()) {
		error_exit("read: QoS");
	}
	if (publish->dup() != publishRead.dup()) {
		error_exit("read: DUP");
	}
	if (publish->topic() != publishRead.topic()) {
		error_exit("read: Topic");
	}
	if (publish->packetId() != publishRead.packetId()) {
		error_exit("read: Packet Id");
	}
	if (publish->payload() != publishRead.payload()) {
		error_exit("read: Payload");
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

