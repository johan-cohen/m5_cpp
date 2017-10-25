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

#include "Properties.hpp"
#include "test_Common.hpp"

#include <cstring>

#define HELLO_WORLD	"Hello, World!"

#define testU16	0xABCD
#define testU8	0xAB
#define testU32	0xABCDEDF1U

int test_PropertiesList()
{
	m5::PropertiesList *propList;
	uint16_t u16;

	/* xxx */
	propList = new m5::PropertiesList(m5::PktType::PUBLISH);

	propList->payloadFormatIndicator(testU8);
	uint8_t u8 = propList->payloadFormatIndicator();
	if (u8 != testU8) {
		throw std::logic_error("payloadFormatIndicator");
	}

	propList->publicationExpiryInterval(testU32);
	uint32_t u32 = propList->publicationExpiryInterval();
	if (u32 != testU32) {
		throw std::logic_error("publicationExpiryInterval");
	}

	propList->topicAlias(testU16);
	u16 = propList->topicAlias();
	if (u16 != testU16) {
		throw std::logic_error("topicAlias");
	}

	propList->contentType(HELLO_WORLD);
	m5::BasicBuf buf = propList->contentType();
	if (buf.size != strlen(HELLO_WORLD) ||
	    memcmp(buf.data, HELLO_WORLD, buf.size) != 0) {
		throw std::logic_error("contentType");
	}

	delete propList;

	return 0;
}

int main(void)
{
	int rc;

	rc = test_PropertiesList();
	test_rc(rc, "PropertiesList");

	return 0;
}
