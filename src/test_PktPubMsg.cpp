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

#include "test_Common.hpp"
#include "PktPubAck.hpp"
#include "PktPubRec.hpp"
#include "PktPubRel.hpp"
#include "PktPubComp.hpp"

template <typename T> int test(void)
{
	uint16_t u16 = 0xABCD;
	m5::AppBuf buf(16);
	T *pub;

	pub = new T();
	pub->packetId(u16);
	pub->reasonCode(m5::ReasonCode::SERVER_BUSY);

	pub->writeTo(buf);
	if (pub->status() != m5::StatusCode::SUCCESS) {
		error_exit("writeTo");
	}

	T pubRead;
	pubRead.readFrom(buf);
	if (pubRead.status() != m5::StatusCode::SUCCESS) {
		error_exit("readFrom");
	}

	if (pub->reasonCode() != pubRead.reasonCode()) {
		error_exit("read: Reason Code");
	}
	if (pub->packetId() != pubRead.packetId()) {
		error_exit("read: Packet Id");
	}

	delete pub;

	return 0;
}

int main(void)
{
	int rc;

	rc = test<m5::PktPubAck>();
	test_rc(rc, "PktPubAck");

	rc = test<m5::PktPubRec>();
	test_rc(rc, "PktPubRec");

	rc = test<m5::PktPubRel>();
	test_rc(rc, "PktPubRel");

	rc = test<m5::PktPubComp>();
	test_rc(rc, "PktPubComp");

	return 0;
}

