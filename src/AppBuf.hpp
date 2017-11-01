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

#ifndef __APPBUF_HPP__
#define __APPBUF_HPP__

#include <cstdint>
#include <cstddef>
#include <vector>

namespace m5 {

class AppBuf {
private:
	std::vector<uint8_t> _data;

	std::size_t _offset = 0;
	std::size_t _length = 0;
	std::size_t _size = 0;

	void init(std::size_t size);
public:
	const uint8_t *data() const { return _data.data(); }

	uint8_t *currentWrite(void) { return _data.data() + _length; }
	uint8_t *currentRead(void) { return _data.data() + _offset; }

	AppBuf(const uint8_t *data, std::size_t size);
	AppBuf(std::size_t size = 0);
	~AppBuf();

	std::size_t size(void) const { return _size; }
	std::size_t length(void) const { return _length; }
	std::size_t traversed(void) const { return _offset; }
	void rewind();
	void reset(void);

	std::size_t bytesToRead(void) const;

	/* createFrom reads size bytes from src to create a new AppBuf */
	static AppBuf *createFrom(AppBuf &src, std::size_t bytesToRead);

	template <typename T> T readNum(void);

	/* read does not do bound checking, use bytesToRead */
	void read(uint8_t *d, std::size_t size);

	/* readNum8 does not do bound checking, use bytesToRead */
	uint8_t readNum8(void);

	/* readNum16 does not do bound checking, use bytesToRead */
	uint16_t readNum16(void);

	/* readNum32 does not do bound checking, use bytesToRead */
	uint32_t readNum32(void);

	void readBinary(std::vector<uint8_t> &dst);

	void readVBI(uint32_t &v, uint8_t &wireSize);
	uint32_t readVBI(void);

	/* readSkip does not do bound checking, use bytesToRead */
	void readSkip(std::size_t n, bool forward = true);

	std::size_t bytesToWrite(void) const;

	template <typename T> void writeNum(const T &v);

	/* write does not do bound checking, use bytesToWrite */
	void write(const uint8_t *d, std::size_t size);

	/* writeNum8 does not do bound checking, use bytesToWrite */
	void writeNum8(uint8_t v);

	/* writeNum16 does not do bound checking, use bytesToWrite */
	void writeNum16(uint16_t v);

	/* writeNum32 does not do bound checking, use bytesToWrite */
	void writeNum32(uint32_t v);

	void writeBinary(const uint8_t *data, uint16_t size);
	void writeBinary(const std::vector<uint8_t> &src);
	void writeString(const char *str);

	void writeVBI(uint32_t v);
};

}

#endif

