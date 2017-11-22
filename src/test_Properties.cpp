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
#include "Properties.hpp"
#include "Common.hpp"

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
	m5::StatusCode rc;
	uint32_t u32;
	uint16_t u16;
	bool res;

	propList = new m5::Properties(m5::PktType::PUBLISH);

	rc = propList->payloadFormatIndicator(true);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("payloadFormatIndicator");
	}
	res = propList->payloadFormatIndicator();
	if (res != true) {
		error_exit("payloadFormatIndicator");
	}
	wireSize += m5::propertyIdSize + 1;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: payloadFormatIndicator");
	}

	rc = propList->publicationExpiryInterval(testU32);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("publicationExpiryInterval");
	}
	u32 = propList->publicationExpiryInterval();
	if (u32 != testU32) {
		error_exit("publicationExpiryInterval");
	}
	wireSize += m5::propertyIdSize + 4;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: publicationExpiryInterval");
	}

	rc = propList->topicAlias(testU16);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("topicAlias");
	}
	u16 = propList->topicAlias();
	if (u16 != testU16) {
		error_exit("topicAlias");
	}
	wireSize += m5::propertyIdSize + 2;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: topicAlias");
	}

	rc = propList->responseTopic(MSG);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("responseTopic");
	}
	if (msg != propList->responseTopic()) {
		error_exit("responseTopic");
	}
	wireSize += m5::propertyIdSize + m5::stringLenSize + MSG_LEN;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: responseTopic");
	}

	/* rewrite property value */
	rc = propList->responseTopic("hello");
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("responseTopic");
	}
	if (msg == propList->responseTopic()) {
		error_exit("responseTopic");
	}
	wireSize += -MSG_LEN + 5;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: responseTopic");
	}

	rc = propList->correlationData(DATA, DATA_LEN);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("correlationData");
	}
	if (msg != propList->correlationData()) {
		error_exit("correlationData");
	}
	wireSize += m5::propertyIdSize + m5::binaryLenSize + DATA_LEN;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: correlationData");
	}

	rc = propList->subscriptionIdentifier(testU32);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("subscriptionIdentifier");
	}
	u32 = propList->subscriptionIdentifier();
	if (u32 != testU32) {
		error_exit("subscriptionIdentifier");
	}
	wireSize += m5::propertyIdSize + m5::VBIWireSize(testU32);
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: subscriptionIdentifier");
	}

	rc = propList->contentType(MSG);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("contentType");
	}
	if (msg != propList->contentType()) {
		error_exit("contentType");
	}
	wireSize += m5::propertyIdSize + m5::stringLenSize + MSG_LEN;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: contentType");
	}

	delete propList;

	propList = new m5::Properties(m5::PktType::CONNECT);

	rc = propList->sessionExpiryInterval(testU32);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("sessionExpiryInterval");
	}
	u32 = propList->sessionExpiryInterval();
	if (u32 != testU32) {
		error_exit("sessionExpiryInterval");
	}
	wireSize = m5::propertyIdSize + 4;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: sessionExpiryInterval");
	}

	rc = propList->willDelayInterval(testU32);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("willDelayInterval");
	}
	u32 = propList->willDelayInterval();
	if (u32 != testU32) {
		error_exit("willDelayInterval");
	}
	wireSize += m5::propertyIdSize + 4;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: willDelayInterval");
	}

	rc = propList->receiveMaximum(testU16);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("receiveMaximum");
	}
	u16 = propList->receiveMaximum();
	if (u16 != testU16) {
		error_exit("receiveMaximum");
	}
	wireSize += m5::propertyIdSize + 2;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: receiveMaximum");
	}

	rc = propList->maximumPacketSize(testU32);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("maximumPacketSize");
	}
	u32 = propList->maximumPacketSize();
	if (u32 != testU32) {
		error_exit("maximumPacketSize");
	}
	wireSize += m5::propertyIdSize + 4;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: maximumPacketSize");
	}

	rc = propList->topicAliasMaximum(testU16);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("topicAliasMaximum");
	}
	u16 = propList->topicAliasMaximum();
	if (u16 != testU16) {
		error_exit("topicAliasMaximum");
	}
	wireSize += m5::propertyIdSize + 2;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: topicAliasMaximum");
	}

	rc = propList->requestResponseInformation(true);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("requestResponseInformation");
	}
	res = propList->requestResponseInformation();
	if (!res) {
		error_exit("requestResponseInformation");
	}
	wireSize += m5::propertyIdSize + 1;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: requestResponseInformation");
	}

	rc = propList->requestProblemInformation(true);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("requestProblemInformation");
	}
	res = propList->requestProblemInformation();
	if (!res) {
		error_exit("requestProblemInformation");
	}
	wireSize += m5::propertyIdSize + 1;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: requestProblemInformation");
	}

	rc = propList->authenticationMethod(MSG);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("authenticationMethod");
	}
	if (msg != propList->authenticationMethod()) {
		error_exit("authenticationMethod");
	}
	wireSize += m5::propertyIdSize + m5::stringLenSize + MSG_LEN;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: authenticationMethod");
	}

	rc = propList->authenticationData(DATA, DATA_LEN);
	if (rc != m5::StatusCode::SUCCESS) {
		error_exit("authenticationData");
	}
	if (msg != propList->authenticationData()) {
		error_exit("authenticationData");
	}
	wireSize += m5::propertyIdSize + m5::binaryLenSize + DATA_LEN;
	if (propList->wireSize() != wireSize) {
		error_exit("wireSize: authenticationData");
	}

	int i = 0;
	while (KeyVal[i].key != nullptr) {
		rc = propList->userProperty(KeyVal[i].key, KeyVal[i].val);
		if (rc != m5::StatusCode::SUCCESS) {
			error_exit("userProperty");
		}

		wireSize += m5::propertyIdSize +
			    m5::stringLenSize + strlen(KeyVal[i].key) +
			    m5::stringLenSize + strlen(KeyVal[i].val);
		if (propList->wireSize() != wireSize) {
			error_exit("wireSize: userProperty");
		}

		i += 1;
	}

	auto userProps = propList->userProperty();

	int j = 0;
	for (auto it = userProps.begin(); it != userProps.end(); it++) {
		const m5::ByteArray *key = (*it).first;
		if (strlen(KeyVal[j].key) != key->size()) {
			error_exit("userProperty key len");
		}
		if (memcmp(key->data(), KeyVal[j].key, key->size()) != 0) {
			error_exit("userProperty key");
		}

		const m5::ByteArray *value = (*it).second;
		if (strlen(KeyVal[j].val) != value->size()) {
			error_exit("userProperty val len");
		}
		if (memcmp(value->data(), KeyVal[j].val, value->size()) != 0) {
			error_exit("userProperty val");
		}

		j += 1;
	}

	if (i != j) {
		error_exit("userProperty");
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

