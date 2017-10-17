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

#include <stdexcept>
#include <cstring>

namespace m5 {

void AppBuf::init(std::size_t size)
{
	if (size > 0) {
		this->data = new uint8_t[size];
	} else {
		this->data = nullptr;
	}

	this->maxSize = size;
	this->len = 0;
	this->offset = 0;
}

AppBuf::AppBuf(const uint8_t *data, std::size_t size)
{
	init(size);

	memcpy(this->data, data, size);
	this->len = size;
}

AppBuf::AppBuf(AppBuf &src, std::size_t size)
{
	if (src.bytesToRead() < size) {
		throw std::out_of_range("No enough space in input buffer");
	}

	init(size);

	src.read(this->data, size);
	this->len = size;
}

AppBuf::AppBuf(std::size_t size)
{
	init(size);
}

AppBuf::~AppBuf()
{
	if (this->data != nullptr) {
		delete[] this->data;
	}
}

void AppBuf::rewind(void)
{
	this->offset = 0;
}

void AppBuf::reset(void)
{
	this->offset = 0;
	this->len = 0;
}

std::size_t AppBuf::bytesToRead(void) const
{
	return this->len - this->offset;
}

void AppBuf::read(uint8_t *d, std::size_t size)
{
	memcpy(d, this->data + this->offset, size);
	this->offset += size;
}

template <typename T> T AppBuf::_read(void)
{
	uint8_t *d;
	T v = 0;

	d = reinterpret_cast<uint8_t *>(&v);
	read(d, sizeof(T));

	return v;
}

uint8_t AppBuf::readNum8(void)
{
	return _read<uint8_t>();
}

uint16_t AppBuf::readNum16(void)
{
	return be16toh(_read<uint16_t>());
}

uint32_t AppBuf::readNum32(void)
{
	return be32toh(_read<uint32_t>());
}

void AppBuf::readBinary(uint8_t *data, uint16_t &len, uint16_t size)
{
	/* two bytes for the length, length could be 0... */
	if (bytesToRead() < 2) {
		throw std::out_of_range("No enough space in input buffer");
	}

	len = this->readNum16();
	if (len == 0) {
		return;
	}

	if (len > bytesToRead()) {
		throw std::out_of_range("No enough space in input buffer");
	}

	if (len > size) {
		throw std::out_of_range("No enough space in output buffer");
	}

	memcpy(data, this->data + this->offset, len);
	this->offset += len;
}

void AppBuf::readVBI(uint32_t &v, uint8_t &wireSize)
{
	uint32_t multiplier = 1;
	uint8_t encoded;

	v = 0;
	wireSize = 0;
	do {
		if (bytesToRead() < 1) {
			throw std::out_of_range("No enough space in buffer");
		}

		if (multiplier > 128 * 128 * 128) {
			throw std::invalid_argument("Error in input bytes");
		}

		encoded = readNum8();
		wireSize += 1;

		v += (encoded & 127) * multiplier;
		multiplier *= 128;
	} while ((encoded & 128) != 0);
}

uint32_t AppBuf::readVBI(void)
{
	uint8_t wireSize;
	uint32_t v;

	readVBI(v, wireSize);
	(void)wireSize;

	return v;
}

void AppBuf::readSkip(std::size_t n, bool forward)
{
	this->offset += (forward ? 1 : -1) * n;
}

void AppBuf::write(const uint8_t *d, std::size_t size)
{
	memcpy(this->data + this->len, d, size);
	this->len += size;
}

template <typename T> T host2net(T v)
{
	switch (sizeof(v)) {
	case 1: return v;
	case 2: return htobe16(v);
	case 4: return htobe32(v);
	}

	return 0;
}

std::size_t AppBuf::bytesToWrite(void) const
{
	return this->maxSize - this->len;
}

template <typename T> void AppBuf::_write(const T &v)
{
	uint8_t *data;
	T vv;

	vv = host2net<T>(v);
	data = reinterpret_cast<uint8_t *>(&vv);

	this->write(data, sizeof(v));
}

void AppBuf::writeNum8(uint8_t v)
{
	this->_write<uint8_t>(v);
}

void AppBuf::writeNum16(uint16_t v)
{
	this->_write<uint16_t>(v);
}

void AppBuf::writeNum32(uint32_t v)
{
	this->_write<uint32_t>(v);
}

void AppBuf::writeBinary(const uint8_t *data, uint16_t size)
{
	this->writeNum16(size);
	this->write(data, size);
}

void AppBuf::writeString(const char *str)
{
	this->writeBinary((uint8_t *)str, strlen(str));
}

void AppBuf::writeVBI(uint32_t v)
{
	do {
		uint8_t encoded;

		if (bytesToWrite() < 1) {
			throw std::invalid_argument("Invalid input argument");
		}

		encoded = v % 128;
		v = v / 128;
		if (v > 0) {
			encoded = encoded | 128;
		}

		writeNum8(encoded);
	} while (v > 0);
}

}
