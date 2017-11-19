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
 * Tests for the MQTT 5 Low Level Packet Library
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

#include "AppBuf.hpp"
#include "test_Common.hpp"

#include <cstring>

int test(void)
{
	const char str[] = "Hello, World!";
	m5::AppBuf *buf;

	buf = new m5::AppBuf(64);

	for (std::size_t i = 0; i < buf->size(); i++) {
		if (buf->bytesToWrite() != (buf->size() - i)) {
			error_exit("bytesToWrite");
		}

		buf->writeNum8((uint8_t)i);
	}

	for (std::size_t i = 0; i < buf->size(); i++) {
		buf->readNum8();
	}

	if (buf->bytesToRead() != 0) {
		error_exit("bytesToRead");
	}

	if (buf->bytesToWrite() != 0) {
		error_exit("bytesToWrite");
	}

	buf->rewind();
	if (buf->bytesToRead() != buf->length()) {
		error_exit("rewind");
	}

	/* Buffer data is 0x00 0x01 0x02 0x03 0x04 0x05 ... */
	if (buf->readNum16() != 0x0001) {
		error_exit("readNum16");
	}

	if (buf->bytesToRead() != (buf->length() - sizeof(uint16_t))) {
		error_exit("readNum16");
	}

	if (buf->readNum32() != 0x02030405) {
		error_exit("readNum32");
	}

	if (buf->bytesToRead() != (buf->length() - (sizeof(uint32_t) + sizeof(uint16_t)))) {
		error_exit("readNum32");
	}

	buf->reset();
	if (buf->length() != 0 || buf->bytesToWrite() != buf->size()) {
		error_exit("reset");
	}

	buf->writeNum16(0x0A0B);
	/* Now buf is 0x0A 0x0B */
	if (buf->readNum16() != 0x0A0B) {
		error_exit("writeNum16");
	}

	buf->writeNum32(0x0C0D0E0F);
	/* Now buf is 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F */
	if (buf->readNum32() != 0x0C0D0E0F) {
		error_exit("writeNum32");
	}

	buf->writeString(str);

	m5::ByteArray v;
	auto rc = buf->readBinary(v);
	if (rc != m5::StatusCode::SUCCESS || v.size() != strlen(str)) {
		error_exit("readBinary len");
	}
	if (memcmp(v.data(), str, strlen(str)) != 0) {
		error_exit("readBinary data");
	}

	delete buf;

	buf = new m5::AppBuf((uint8_t *)str, strlen(str));

	if (buf->size() != strlen(str) || buf->length() != buf->size()) {
		error_exit("size or length");
	}
	if (memcmp(buf->data(), str, buf->length()) != 0) {
		error_exit("rawData");
	}
	if (buf->bytesToRead() != buf->length() || buf->bytesToWrite() != 0) {
		error_exit("bytesToRead or bytesToWrite");
	}

	delete buf;

	buf = new m5::AppBuf(16);
	for (std::size_t i = 0; i < buf->size(); i++) {
		buf->writeNum8(i);
	}

	buf->readSkip(5);
	if (buf->data()[5] != buf->readNum8()) {
		error_exit("readSkip forward");
	}

	buf->readSkip(6, false);
	if (buf->data()[0] != buf->readNum8()) {
		error_exit("readSkip");
	}

	delete buf;

	return 0;
}

int main(void)
{
	int rc;

	rc = test();
	test_rc(rc, "AppBuf");

	return rc;
}

