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

#ifndef __TEST_COMMON_HPP__
#define __TEST_COMMON_HPP__

#include <iostream>
#include <iomanip>
#include <cstdint>

#define error_exit(msg)						\
	std::cout << __func__ << ": " << (msg) << " error"	\
		  << std::endl;					\
	return 1;

#define test_rc(rc, msg)				\
	std::cout << (((rc) != 0) ? "ERROR\t" : "OK\t")	\
		  << (msg) << std::endl;		\

namespace m5 {

static inline void printArray(const uint8_t *data, uint16_t length)
{
	for (uint16_t i = 0; i < length; i++) {
		std::cout << std::hex << std::setfill('0') << std::setw(2)
			  <<  (int)data[i];
		if ((i + 1) % 8 != 0) {
			std::cout << " ";
		} else {
			std::cout << "\n";
		}
	}
	std::cout << "\n";
}

}

#endif

