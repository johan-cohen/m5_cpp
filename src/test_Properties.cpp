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

#define testU32	0x01ABCDEFU
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

int test_Properties(void)
{
	m5::Properties *propList;
	uint32_t wireSize = 0;
	uint32_t u32;
	uint16_t u16;
	uint8_t u8;
	bool res;

	propList = new m5::Properties(m5::PktType::PUBLISH);

	propList->payloadFormatIndicator(testU8);
	u8 = propList->payloadFormatIndicator();
	if (u8 != testU8) {
		throw std::logic_error("payloadFormatIndicator");
	}
	wireSize += m5::propertyIdSize + 1;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: payloadFormatIndicator");
	}

	propList->publicationExpiryInterval(testU32);
	u32 = propList->publicationExpiryInterval();
	if (u32 != testU32) {
		throw std::logic_error("publicationExpiryInterval");
	}
	wireSize += m5::propertyIdSize + 4;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: publicationExpiryInterval");
	}

	propList->topicAlias(testU16);
	u16 = propList->topicAlias();
	if (u16 != testU16) {
		throw std::logic_error("topicAlias");
	}
	wireSize += m5::propertyIdSize + 2;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: topicAlias");
	}

	propList->responseTopic(MSG);
	if (msg != propList->responseTopic()) {
		throw std::logic_error("responseTopic");
	}
	wireSize += m5::propertyIdSize + m5::stringLenSize + MSG_LEN;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: responseTopic");
	}

	/* rewrite property value */
	propList->responseTopic("hello");
	if (msg == propList->responseTopic()) {
		throw std::logic_error("responseTopic");
	}
	wireSize += -MSG_LEN + 5;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: responseTopic");
	}

	propList->correlationData(DATA, DATA_LEN);
	if (msg != propList->correlationData()) {
		throw std::logic_error("correlationData");
	}
	wireSize += m5::propertyIdSize + m5::binaryLenSize + DATA_LEN;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: correlationData");
	}

	propList->subscriptionIdentifier(testU32);
	u32 = propList->subscriptionIdentifier();
	if (u32 != testU32) {
		throw std::logic_error("subscriptionIdentifier");
	}
	wireSize += m5::propertyIdSize + m5::VBIWireSize(testU32);
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: subscriptionIdentifier");
	}

	propList->contentType(MSG);
	if (msg != propList->contentType()) {
		throw std::logic_error("contentType");
	}
	wireSize += m5::propertyIdSize + m5::stringLenSize + MSG_LEN;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: contentType");
	}

	delete propList;

	propList = new m5::Properties(m5::PktType::CONNECT);

	propList->sessionExpiryInterval(testU32);
	u32 = propList->sessionExpiryInterval();
	if (u32 != testU32) {
		throw std::logic_error("sessionExpiryInterval");
	}
	wireSize = m5::propertyIdSize + 4;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: sessionExpiryInterval");
	}

	propList->willDelayInterval(testU32);
	u32 = propList->willDelayInterval();
	if (u32 != testU32) {
		throw std::logic_error("willDelayInterval");
	}
	wireSize += m5::propertyIdSize + 4;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: willDelayInterval");
	}

	propList->receiveMaximum(testU16);
	u16 = propList->receiveMaximum();
	if (u16 != testU16) {
		throw std::logic_error("receiveMaximum");
	}
	wireSize += m5::propertyIdSize + 2;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: receiveMaximum");
	}

	propList->maximumPacketSize(testU32);
	u32 = propList->maximumPacketSize();
	if (u32 != testU32) {
		throw std::logic_error("maximumPacketSize");
	}
	wireSize += m5::propertyIdSize + 4;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: maximumPacketSize");
	}

	propList->topicAliasMaximum(testU16);
	u16 = propList->topicAliasMaximum();
	if (u16 != testU16) {
		throw std::logic_error("topicAliasMaximum");
	}
	wireSize += m5::propertyIdSize + 2;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: topicAliasMaximum");
	}

	propList->requestResponseInformation(true);
	res = propList->requestResponseInformation();
	if (!res) {
		throw std::logic_error("requestResponseInformation");
	}
	wireSize += m5::propertyIdSize + 1;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: requestResponseInformation");
	}

	propList->requestProblemInformation(true);
	res = propList->requestProblemInformation();
	if (!res) {
		throw std::logic_error("requestProblemInformation");
	}
	wireSize += m5::propertyIdSize + 1;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: requestProblemInformation");
	}

	propList->authenticationMethod(MSG);
	if (msg != propList->authenticationMethod()) {
		throw std::logic_error("authenticationMethod");
	}
	wireSize += m5::propertyIdSize + m5::stringLenSize + MSG_LEN;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: authenticationMethod");
	}

	propList->authenticationData(DATA, DATA_LEN);
	if (msg != propList->authenticationData()) {
		throw std::logic_error("authenticationData");
	}
	wireSize += m5::propertyIdSize + m5::binaryLenSize + DATA_LEN;
	if (propList->wireSize() != wireSize) {
		throw std::logic_error("wireSize: authenticationData");
	}

	int i = 0;
	while (KeyVal[i].key != nullptr) {
		propList->userProperty(KeyVal[i].key, KeyVal[i].val);

		wireSize += m5::propertyIdSize +
			    m5::stringLenSize + strlen(KeyVal[i].key) +
			    m5::stringLenSize + strlen(KeyVal[i].val);
		if (propList->wireSize() != wireSize) {
			throw std::logic_error("wireSize: userProperty");
		}

		i += 1;
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

		j += 1;
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

	rc = test_Properties();
	test_rc(rc, "Properties");

	return 0;
}

