#
#                                   1byt3
#
#                              License Notice
#
# 1byt3 provides a commercial license agreement for this software. This
# commercial license can be used for development of proprietary/commercial
# software. Under this commercial license you do not need to comply with the
# terms of the GNU Affero General Public License, either version 3 of the
# License, or (at your option) any later version.
#
# If you don't receive a commercial license from us (1byt3), you MUST assume
# that this software is distributed under the GNU Affero General Public
# License, either version 3 of the License, or (at your option) any later
# version.
#
# Contact us for additional information: customers at 1byt3.com
#
#                          End of License Notice
#

#
# MQTT 5 Low Level Packet Library
#
# Copyright (C) 2017 1byt3, customers at 1byt3.com
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

CPPFLAGS =					\
	-std=c++11 -Wall -Wextra -Werror

INC = -Isrc

EXTRAFLAGS = -O0 -g

CXX ?= g++

SRCS_DIR = src

OBJS_DIR = obj

BINS_DIR = bin

TESTS =					\
	$(BINS_DIR)/test_AppBuf		\
	$(BINS_DIR)/test_Properties	\
	$(BINS_DIR)/test_PktConnect	\
	$(BINS_DIR)/test_PktConnAck	\
	$(BINS_DIR)/test_PktPublish	\
	$(BINS_DIR)/test_PktPubMsg	\
	$(BINS_DIR)/test_PktRCodeProp	\
	$(BINS_DIR)/test_PktPing	\
	$(BINS_DIR)/test_PktSubscribe	\
	$(BINS_DIR)/test_PktSubAckMsg	\
	$(BINS_DIR)/test_PktUnsubscribe

VALGRIND = valgrind -q --leak-check=full --error-exitcode=1

all: dirs $(TESTS)

dirs:
	@mkdir -p obj
	@mkdir -p bin

$(OBJS_DIR)/test_%.o:			\
	$(SRCS_DIR)/test_%.cpp		\
	$(SRCS_DIR)/%.cpp		\
	$(SRCS_DIR)/%.hpp		\
	$(SRCS_DIR)/AppBuf.hpp		\
	$(SRCS_DIR)/Packet.hpp		\
	$(SRCS_DIR)/test_Common.hpp	\
	$(SRCS_DIR)/Properties.hpp	\
	$(SRCS_DIR)/Common.hpp
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) $(INC) -c -o $@ $<

$(OBJS_DIR)/%.o:			\
	$(SRCS_DIR)/%.cpp		\
	$(SRCS_DIR)/%.hpp
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) $(INC) -c -o $@ $<

$(BINS_DIR)/test_PktPubMsg:		\
	$(OBJS_DIR)/test_PktPubMsg.o	\
	$(OBJS_DIR)/PktPubMsg.o		\
	$(OBJS_DIR)/PktPubAck.o		\
	$(OBJS_DIR)/PktPubRec.o		\
	$(OBJS_DIR)/Properties.o	\
	$(OBJS_DIR)/AppBuf.o		\
	$(OBJS_DIR)/Packet.o
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -o $@ $^

$(BINS_DIR)/test_PktRCodeProp:		\
	$(OBJS_DIR)/test_PktRCodeProp.o	\
	$(OBJS_DIR)/PktRCodeProp.o	\
	$(OBJS_DIR)/PktAuth.o		\
	$(OBJS_DIR)/PktDisconnect.o	\
	$(OBJS_DIR)/Properties.o	\
	$(OBJS_DIR)/AppBuf.o		\
	$(OBJS_DIR)/Packet.o
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -o $@ $^

$(BINS_DIR)/test_%:			\
	$(OBJS_DIR)/test_%.o		\
	$(OBJS_DIR)/%.o			\
	$(OBJS_DIR)/Properties.o	\
	$(OBJS_DIR)/AppBuf.o		\
	$(OBJS_DIR)/Packet.o
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -o $@ $^

tests: $(TESTS)
	@$(foreach test, $(TESTS), ./$(test) || exit 1;)

memtest: $(TESTS)
	@$(foreach test, $(TESTS), $(VALGRIND) ./$(test) || exit 1;)

clean:
	rm -rf obj bin

.PHONY: all dirs tests memtest clean
