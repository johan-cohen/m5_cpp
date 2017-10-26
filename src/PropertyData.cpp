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

#include "PropertyData.hpp"

#include <cstring>

namespace m5 {

void PropertyData::release(void)
{
	if (!useNum) {
		delete[] _data;
	}

	_data = nullptr;
	scalar = false;
	useNum = false;
	_size = 0;
	num = 0;
}

void PropertyData::init(const uint8_t *data, uint16_t size)
{
	if (size == 0) {
		return;
	}

	useNum = (sizeof(this->num) >= size);
	this->_size = size;

	if (data == nullptr) {
		return;
	}

	if (useNum) {
		this->_data = (uint8_t *)&(this->num);
	} else {
		this->_data = new uint8_t[size];
	}

	memcpy(this->_data, data, size);
}

void PropertyData::init(uint64_t v)
{
	this->_data = (uint8_t *)&(this->num);
	this->_size = sizeof(this->num);
	this->useNum = true;
	this->scalar = true;
	this->num = v;
}

PropertyData::PropertyData(const uint8_t *data, uint16_t size)
{
	this->init(data, size);
}

PropertyData::PropertyData(uint64_t v)
{
	init(v);
}


PropertyData::~PropertyData()
{
	release();
}

void PropertyData::reset(const uint8_t *data, uint16_t size)
{
	if (this->size() < size) {
		release();
		init(data, size);
	} else {
		scalar = false;
		this->_size = size;
		memcpy(this->_data, data, size);
	}
}

void PropertyData::reset(uint64_t v)
{
	if (!scalar) {
		release();
	}

	init(v);
}

}
