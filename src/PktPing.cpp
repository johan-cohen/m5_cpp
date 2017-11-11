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

#include "PktPing.hpp"

#include <stdexcept>

namespace m5 {

uint32_t PktPing::writeTo(AppBuf &buf)
{
	if (buf.bytesToWrite() < 2) {
		throw std::out_of_range("No enough space in buffer");
	}

	buf.writeNum8(m5::firstByte(this->_packetType));
	buf.writeNum8(0);

	return 2;
}

uint32_t PktPing::readFrom(AppBuf &buf)
{
	if (buf.bytesToRead() < 2) {
		throw std::out_of_range("No enough space in buffer");
	}

	if (buf.readNum8() != m5::firstByte(this->_packetType)) {
		throw std::invalid_argument("PING msg not found in buf");
	}

	if (buf.readNum8() != 0) {
		throw std::invalid_argument("Invalid Remaining Length found");
	}

	return 2;
}

}

