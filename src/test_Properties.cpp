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
#define DATA		(const uint8_t *)HELLO_WORLD
#define DATA_LEN	strlen(HELLO_WORLD)

#define testU32	0xABCDEDF1U
#define testU16	0xABCDU
#define testU8	0xABU

int cmp_data(const m5::BasicBuf &buf, const uint8_t *d, std::size_t size)
{
	if (buf.size != size) {
		return 1;
	}

	return memcmp(buf.data, d, size);
}

int cmp_str(const m5::BasicBuf &buf, const char *str)
{
	return cmp_data(buf, (const uint8_t *)str, strlen(str));
}

int test_PropertiesList()
{
	m5::PropertiesList *propList;
	m5::BasicBuf buf;
	uint32_t u32;
	uint16_t u16;
	uint8_t u8;
	bool res;

	propList = new m5::PropertiesList(m5::PktType::PUBLISH);

	propList->payloadFormatIndicator(testU8);
	u8 = propList->payloadFormatIndicator();
	if (u8 != testU8) {
		throw std::logic_error("payloadFormatIndicator");
	}

	propList->publicationExpiryInterval(testU32);
	u32 = propList->publicationExpiryInterval();
	if (u32 != testU32) {
		throw std::logic_error("publicationExpiryInterval");
	}

	propList->topicAlias(testU16);
	u16 = propList->topicAlias();
	if (u16 != testU16) {
		throw std::logic_error("topicAlias");
	}

	propList->responseTopic(HELLO_WORLD);
	buf = propList->responseTopic();
	if (cmp_str(buf, HELLO_WORLD) != 0) {
		throw std::logic_error("responseTopic");
	}

	propList->correlationData(DATA, DATA_LEN);
	buf = propList->responseTopic();
	if (cmp_data(buf, DATA, DATA_LEN) != 0) {
		throw std::logic_error("correlationData");
	}

	propList->subscriptionIdentifier(testU32);
	u32 = propList->subscriptionIdentifier();
	if (u32 != testU32) {
		throw std::logic_error("subscriptionIdentifier");
	}

	propList->contentType(HELLO_WORLD);
	buf = propList->contentType();
	if (cmp_str(buf, HELLO_WORLD) != 0) {
		throw std::logic_error("contentType");
	}

	delete propList;

	propList = new m5::PropertiesList(m5::PktType::CONNECT);

	propList->sessionExpiryInterval(testU32);
	u32 = propList->sessionExpiryInterval();
	if (u32 != testU32) {
		throw std::logic_error("sessionExpiryInterval");
	}

	propList->willDelayInterval(testU32);
	u32 = propList->willDelayInterval();
	if (u32 != testU32) {
		throw std::logic_error("willDelayInterval");
	}

	propList->receiveMaximum(testU16);
	u16 = propList->receiveMaximum();
	if (u16 != testU16) {
		throw std::logic_error("receiveMaximum");
	}

	propList->maximumPacketSize(testU32);
	u32 = propList->maximumPacketSize();
	if (u32 != testU32) {
		throw std::logic_error("maximumPacketSize");
	}

	propList->topicAliasMaximum(testU16);
	u16 = propList->topicAliasMaximum();
	if (u16 != testU16) {
		throw std::logic_error("topicAliasMaximum");
	}

	propList->requestResponseInformation(true);
	res = propList->requestResponseInformation();
	if (!res) {
		throw std::logic_error("requestResponseInformation");
	}

	propList->requestProblemInformation(true);
	res = propList->requestProblemInformation();
	if (!res) {
		throw std::logic_error("requestProblemInformation");
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
