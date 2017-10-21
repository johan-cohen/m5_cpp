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

#define __POW2(n) (uint64_t)(((uint64_t)1) << (n))

namespace  m5 {

void PropertiesList::push(PropertyNode *node)
{
	propList.insert(std::pair<uint8_t, PropertyNode *>(node->id(), node));
}

void PropertiesList::deleteList(void)
{
	std::multimap<uint8_t, PropertyNode *>::iterator it;

	it = propList.begin();
	while (it != propList.end()) {
		struct PropertyNode *node = (*it).second;
		delete node;

		it++;
	}
}

void PropertiesList::computePktFlags(void)
{
	properties = 0;
	enabledProperties = 0;

	switch (pktType) {
	case PktType::CONNECT:
		properties += __POW2(PropertyId::SESSION_EXPIRY_INTERVAL);
		properties += __POW2(PropertyId::AUTH_METHOD);
		properties += __POW2(PropertyId::AUTH_DATA);
		properties += __POW2(PropertyId::REQUEST_PROBLEM_INFORMATION);
		properties += __POW2(PropertyId::WILL_DELAY_INTERVAL);
		properties += __POW2(PropertyId::REQUEST_RESPONSE_INFORMATION);
		properties += __POW2(PropertyId::RECEIVE_MAXIMUM);
		properties += __POW2(PropertyId::TOPIC_ALIAS_MAXIMUM);
		properties += __POW2(PropertyId::USER_PROPERTY);
		properties += __POW2(PropertyId::MAXIMUM_PACKET_SIZE);
		break;
	case PktType::CONNACK:
		properties += __POW2(PropertyId::ASSIGNED_CLIENT_IDENTIFIER);
		properties += __POW2(PropertyId::SERVER_KEEP_ALIVE);
		properties += __POW2(PropertyId::AUTH_METHOD);
		properties += __POW2(PropertyId::AUTH_DATA);
		properties += __POW2(PropertyId::RESPONSE_INFORMATION);
		properties += __POW2(PropertyId::SERVER_REFERENCE);
		properties += __POW2(PropertyId::REASON_STR);
		properties += __POW2(PropertyId::RECEIVE_MAXIMUM);
		properties += __POW2(PropertyId::TOPIC_ALIAS_MAXIMUM);
		properties += __POW2(PropertyId::MAXIMUM_QOS);
		properties += __POW2(PropertyId::RETAIN_AVAILABLE);
		properties += __POW2(PropertyId::USER_PROPERTY);
		properties += __POW2(PropertyId::MAXIMUM_PACKET_SIZE);
		properties += __POW2(PropertyId::WILDCARD_SUBSCRIPTION_AVAILABLE);
		properties += __POW2(PropertyId::SUBSCRIPTION_IDENTIFIER_AVAILABLE);
		properties += __POW2(PropertyId::SHARED_SUBSCRIPTION_AVAILABLE);
		break;
	case PktType::PUBLISH:
		break;
	case PktType::PUBACK:
		break;
	case PktType::PUBREC:
		break;
	case PktType::PUBREL:
		break;
	case PktType::PUBCOMP:
		break;
	case PktType::SUBSCRIBE:
		break;
	case PktType::SUBACK:
		break;
	case PktType::UNSUBSCRIBE:
		break;
	case PktType::UNSUBACK:
		break;
	case PktType::PINGREQ:
		break;
	case PktType::PINGRESP:
		break;
	case PktType::DISCONNECT:
		break;
	case PktType::AUTH:
		break;
	case PktType::RESERVED:
	default:
		break;
	}
}

PropertiesList::PropertiesList(const PktType type)
{
	this->resetPacketType(type);
}

PropertiesList::~PropertiesList()
{
	deleteList();
}

void PropertiesList::resetPacketType(const PktType type)
{
	deleteList();

	this->pktType = type;
	this->computePktFlags();
}

}
