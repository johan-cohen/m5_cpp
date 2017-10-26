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

#ifndef __PROPERTY_DATA_HPP__
#define __PROPERTY_DATA_HPP__

#include <cstdint>

namespace m5 {

class PropertyData {
private:
	uint8_t *_data = nullptr;
	bool scalar = false;
	bool useNum = false;
	uint16_t _size = 0;
	uint64_t num = 0;

	void init(const uint8_t *data, uint16_t size);
	void init(uint64_t v);
	void release(void);

public:
	PropertyData(const uint8_t *data = nullptr, uint16_t size = 0);
	PropertyData(uint64_t v);
	~PropertyData();

	void reset(const uint8_t *data = nullptr, uint16_t size = 0);
	void reset(uint64_t v);

	bool isNumber(void) const { return scalar; }
	uint64_t toNumber(void) const { return num; }

	uint8_t *data() { return _data; }
	uint16_t size() const { return _size; }
};

}

#endif
