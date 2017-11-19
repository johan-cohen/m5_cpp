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

#include "Packet.hpp"

namespace m5 {

Packet::Packet(enum PktType type, uint8_t fixedHeaderReserved) :
	_packetType(type),
	fixedHeaderReserved(fixedHeaderReserved),
	properties(type)
{
}

uint32_t Packet::writeTo(AppBuf &buf)
{
	const auto initialLength = buf.length();
	uint32_t fullPktSize;
	enum StatusCode rc;
	uint8_t remLenWS;
	uint32_t remLen;

	status(StatusCode::SUCCESS);
	expectedWireSize(0);

	if (hasProperties) {
		uint32_t propWSWS;

		propWSWS = VBIWireSize(properties.wireSize());
		if (propWSWS == 0) {
			status(StatusCode::INVALID_PROPERTY_VBI);
			goto lb_exit;
		}

		variableHeaderSize += propWSWS + properties.wireSize();
	}

	remLen = variableHeaderSize + payloadSize;
	remLenWS  = VBIWireSize(remLen);
	if (remLenWS == 0) {
		status(StatusCode::INVALID_REMLEN_VBI);
		goto lb_exit;
	}

	fullPktSize = 1 + remLenWS + remLen;
	expectedWireSize(fullPktSize);
	if (buf.bytesToWrite() < expectedWireSize()) {
		status(StatusCode::NOT_ENOUGH_SPACE_IN_BUFFER);
		goto lb_exit;
	}

	buf.writeNum8(m5::firstByte(packetType(), fixedHeaderReserved));
	buf.writeVBI(remLen);

	rc = writeVariableHeader(buf);
	if (rc != StatusCode::SUCCESS) {
		status(rc);
		goto lb_exit;
	}

	rc = writePayload(buf);
	if (rc != StatusCode::SUCCESS) {
		status(rc);
		goto lb_exit;
	}

	if (buf.length() - initialLength != fullPktSize) {
		status(StatusCode::FINISHED_WRITING_INVALID_LENGTH);
	}

lb_exit:
	return buf.length() - initialLength;
}

}

