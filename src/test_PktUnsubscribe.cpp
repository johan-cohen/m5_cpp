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

#include "PktUnsubscribe.hpp"
#include "test_Common.hpp"

bool cmp(const std::list<m5::ByteArray *> &a, const std::list<m5::ByteArray *> &b)
{
	if (a.size() != b.size()) {
		return false;
	}

	auto a_it = a.begin();
	auto b_it = b.begin();

	do {
		const m5::ByteArray *aArray = *a_it;
		const m5::ByteArray *bArray = *b_it;

		if (*aArray != *bArray) {
			return false;
		}

		a_it++;
		b_it++;
	} while (a_it != a.end() && b_it != b.end());

	return true;
}

int test(void)
{
	m5::PktUnsubscribe *unsubs;
	const char msg[] = "Hello, World!";
	m5::AppBuf buf(256);

	unsubs = new m5::PktUnsubscribe();
	unsubs->packetId(0xABCD);
	unsubs->append(msg);
	unsubs->append(msg);
	unsubs->append(msg);

	unsubs->writeTo(buf);
	if (unsubs->status() != m5::StatusCode::SUCCESS) {
		error_exit("writeTo");
	}

	m5::PktUnsubscribe unsubsRead(buf);
	if (unsubsRead.status() != m5::StatusCode::SUCCESS) {
		error_exit("readFrom");
	}

	if (unsubs->packetId() != unsubsRead.packetId()) {
		error_exit("read/write: Packet Id");
	}
	if (cmp(unsubs->topics(), unsubsRead.topics()) != true) {
		error_exit("read/write: Topics");
	}

	delete unsubs;

	return 0;
}

int main(void)
{
	int rc;

	rc = test();
	test_rc(rc, "PktUnsubscribe");

	return 0;
}

