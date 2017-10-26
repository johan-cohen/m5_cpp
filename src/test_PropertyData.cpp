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
#include "test_Common.hpp"

#include <stdexcept>
#include <cstring>

uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		   0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

int test(void)
{
	m5::PropertyData *pData;

	pData = new m5::PropertyData(nullptr, 0);
	if (pData->data() != nullptr || pData->size() != 0) {
		throw std::logic_error("PropertyData constructor");
	}
	delete pData;

	pData = new m5::PropertyData(data, 0);
	if (pData->data() != nullptr || pData->size() != 0) {
		throw std::logic_error("PropertyData constructor");
	}
	delete pData;

	for (std::size_t i = 1; i < sizeof(uint64_t); i++) {
		pData = new m5::PropertyData(data, i);

		if (pData->size() != i) {
			throw std::logic_error("PropertyData constructor num size");
		}
		if (memcmp(pData->data(), data, i) != 0) {
			throw std::logic_error("PropertyData constructor num data");
		}

		delete pData;
	}

	pData = new m5::PropertyData(data, sizeof(data));
	if (pData->data() == nullptr || pData->size() != sizeof(data)) {
		throw std::logic_error("PropertyData constructor data");
	}
	if (memcmp(pData->data(), data, sizeof(data)) != 0) {
		throw std::logic_error("PropertyData constructor data");
	}
	delete pData;

	return 0;
}

int test2(void)
{
	uint64_t v = 0xABCDEF0123456789;
	m5::PropertyData *pData;

	pData = new m5::PropertyData(v);
	if (!pData->isNumber()) {
		throw std::logic_error("PropertyData constructor num");
	}

	if (pData->toNumber() != v) {
		throw std::logic_error("PropertyData constructor num");
	}

	pData->reset(data, sizeof(data));
	if (pData->isNumber()) {
		throw std::logic_error("PropertyData constructor data -> num?");
	}
	if (pData->data() == nullptr || pData->size() != sizeof(data)) {
		throw std::logic_error("PropertyData constructor data");
	}
	if (memcmp(pData->data(), data, sizeof(data)) != 0) {
		throw std::logic_error("PropertyData constructor data");
	}
	delete pData;

	return 0;
}

int main(void)
{
	int rc;

	rc = test();
	test_rc(rc, "PropertyData");

	rc = test2();
	test_rc(rc, "PropertyData (2)");

	return 0;
}
