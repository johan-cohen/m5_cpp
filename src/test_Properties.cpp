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

#define MSG		"Hello, World!"
#define MSG_LEN		strlen(MSG)
#define DATA		(uint8_t *)MSG
#define DATA_LEN	MSG_LEN

#define testU32	0xABCDEDF1U
#define testU16	0xABCDU
#define testU8	0xABU

struct key_val {
	const char *key;
	const char *val;
};

/* xxx */
static key_val KeyVal[] = {
	{.key = "Key #1", .val = "Val #1"},
	{.key = "Key #2", .val = "Val #2"},
	{.key = "Key #3", .val = "Val #3"},
	{.key = "Key #4", .val = "Val #4"},
	{.key = "Key #5", .val = "Val #5"},
	{.key = nullptr, .val = nullptr}
};

static const std::vector<uint8_t> msg(MSG, MSG + MSG_LEN);

int test_PropertiesList()
{
	m5::PropertiesList *propList;
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

	propList->responseTopic(MSG);
	if (msg != propList->responseTopic()) {
		throw std::logic_error("responseTopic");
	}

	/* rewrite property value */
	propList->responseTopic("hello");
	if (msg == propList->responseTopic()) {
		throw std::logic_error("responseTopic");
	}

	propList->correlationData(DATA, DATA_LEN);
	if (msg != propList->correlationData()) {
		throw std::logic_error("correlationData");
	}

	propList->subscriptionIdentifier(testU32);

	propList->contentType(MSG);
	if (msg != propList->contentType()) {
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

	propList->authenticationMethod(MSG);
	if (msg != propList->authenticationMethod()) {
		throw std::logic_error("authenticationMethod");
	}

	propList->authenticationData(DATA, DATA_LEN);
	if (msg != propList->authenticationData()) {
		throw std::logic_error("authenticationData");
	}


	int i = 0;
	while (KeyVal[i].key != nullptr) {
		propList->userProperty(KeyVal[i].key, KeyVal[i].val);

		i++;
	}

	auto userProps = propList->userProperty();

	int j = 0;
	for (auto it = userProps.begin(); it != userProps.end(); it++) {
		auto pair = (*it);

		if (strlen(KeyVal[j].key) != pair.first.size()) {
			throw std::logic_error("userProperty key len");
		}
		if (memcmp(&pair.first[0], KeyVal[j].key, pair.first.size()) != 0) {
			throw std::logic_error("userProperty key");
		}

		if (strlen(KeyVal[j].val) != pair.second.size()) {
			throw std::logic_error("userProperty val len");
		}
		if (memcmp(&pair.second[0], KeyVal[j].val, pair.second.size()) != 0) {
			throw std::logic_error("userProperty val");
		}

		j++;
	}

	if (i != j) {
		throw std::logic_error("userProperty");
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

