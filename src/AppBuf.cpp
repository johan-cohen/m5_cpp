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

#include "AppBuf.hpp"

#include <endian.h>
#include <cstring>

namespace m5 {

AppBuf::AppBuf(const uint8_t *data, std::size_t size) : _data(data, data + size)
{
	this->_size = size;
	this->_length = size;
}

AppBuf::AppBuf(std::size_t size) : _data(size)
{
	this->_size = size;
}

AppBuf::~AppBuf()
{
}

void AppBuf::rewind(void)
{
	this->_offset = 0;
}

void AppBuf::reset(void)
{
	this->_offset = 0;
	this->_length = 0;
}

std::size_t AppBuf::bytesToRead(void) const
{
	return this->_length - this->_offset;
}

void AppBuf::read(ByteArray &dst, std::size_t size)
{
	dst.insert(dst.end(), this->ptrRead(), this->ptrRead() + size);
	this->_offset += size;
}

void AppBuf::read(uint8_t *d, std::size_t size)
{
	memcpy(d, ptrRead(), size);
	this->_offset += size;
}

template <typename T> T AppBuf::readNum(void)
{
	uint8_t *d;
	T v = 0;

	d = reinterpret_cast<uint8_t *>(&v);
	read(d, sizeof(T));

	switch (sizeof(T)) {
	case 1: return v;
	case 2: return be16toh(v);
	case 4: return be32toh(v);
	case 8: return be64toh(v);
	}

	return v;
}

uint8_t AppBuf::readNum8(void)
{
	return readNum<uint8_t>();
}

uint16_t AppBuf::readNum16(void)
{
	return readNum<uint16_t>();
}

uint32_t AppBuf::readNum32(void)
{
	return readNum<uint32_t>();
}

enum StatusCode AppBuf::readBinary(ByteArray &dst)
{
	/* two bytes for the length, length could be 0... */
	if (bytesToRead() < 2) {
		return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
	}

	auto len = this->readNum16();
	if (len == 0) {
		dst.clear();

		return StatusCode::SUCCESS;
	}

	if (len > bytesToRead()) {
		return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
	}

	dst.assign(ptrRead(), ptrRead() + len);
	this->_offset += len;

	return StatusCode::SUCCESS;
}

ByteArray *AppBuf::readBinary(void)
{
	ByteArray *data = new ByteArray();

	enum StatusCode rc = readBinary(*data);
	(void)rc;

	return data;
}

enum StatusCode AppBuf::readKeyValue(ByteArray &key, ByteArray &value)
{
	enum StatusCode rc;

	rc = readBinary(key);
	if (rc != StatusCode::SUCCESS) {
		return rc;
	}

	rc = readBinary(value);
	if (rc != StatusCode::SUCCESS) {
		return rc;
	}

	return StatusCode::SUCCESS;
}

enum StatusCode AppBuf::readVBI(uint32_t &v, uint8_t &wireSize)
{
	uint32_t multiplier = 1;
	uint8_t encoded;

	v = 0;
	wireSize = 0;
	do {
		if (bytesToRead() < 1) {
			return StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER;
		}

		if (multiplier > 128 * 128 * 128) {
			return StatusCode::INVALID_VBI;
		}

		encoded = readNum8();
		wireSize += 1;

		v += (encoded & 127) * multiplier;
		multiplier *= 128;
	} while ((encoded & 128) != 0);

	return StatusCode::SUCCESS;
}

void AppBuf::readSkip(std::size_t n, bool forward)
{
	this->_offset += (forward ? 1 : -1) * n;
}

void AppBuf::write(const uint8_t *d, std::size_t size)
{
	memcpy(ptrWrite(), d, size);
	this->_length += size;
}

template <typename T> T host2net(T v)
{
	switch (sizeof(v)) {
	case 1: return v;
	case 2: return htobe16(v);
	case 4: return htobe32(v);
	case 8: return htobe64(v);
	}

	return 0;
}

std::size_t AppBuf::bytesToWrite(void) const
{
	return this->_size - this->_length;
}

template <typename T> void AppBuf::writeNum(const T &v)
{
	uint8_t *data;
	T vv;

	vv = host2net<T>(v);
	data = reinterpret_cast<uint8_t *>(&vv);

	this->write(data, sizeof(v));
}

void AppBuf::writeNum8(uint32_t v)
{
	this->writeNum<uint8_t>(v);
}

void AppBuf::writeNum16(uint32_t v)
{
	this->writeNum<uint16_t>(v);
}

void AppBuf::writeNum32(uint32_t v)
{
	this->writeNum<uint32_t>(v);
}

void AppBuf::writeBinary(const uint8_t *data, uint16_t size)
{
	this->writeNum16(size);
	this->write(data, size);
}

void AppBuf::writeBinary(const ByteArray &src)
{
	this->writeBinary(src.data(), src.size());
}

void AppBuf::writeString(const char *str)
{
	this->writeBinary((uint8_t *)str, strlen(str));
}

void AppBuf::writeVBI(uint32_t v)
{
	do {
		uint8_t encoded = v % 128;
		v = v / 128;
		if (v > 0) {
			encoded = encoded | 128;
		}
		writeNum8(encoded);
	} while (v > 0);
}

}

