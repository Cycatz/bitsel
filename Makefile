# Copyright (c) 2022, Fan Chung <cycatz@staque.xyz>
# Copyright (c) 2020, Yung-Yu Chen <yyc@solvcon.net>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
# - Neither the name of the copyright holder nor the names of its contributors
#   may be used to endorse or promote products derived from this software without
#   specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Build bitsel Python extension (even when the timestamp is clean):
#   make
# Build verbosely:
#   make VERBOSE=1
# Build with clang-tidy
#   make USE_CLANG_TIDY=ON

SETUP_FILE ?= ./setup.mk

ifneq (,$(wildcard $(SETUP_FILE)))
	include $(SETUP_FILE)
endif

SKIP_PYTHON_EXECUTABLE ?= OFF
HIDE_SYMBOL ?= OFF
DEBUG_SYMBOL ?= ON
BITSEL_PROFILE ?= OFF
USE_CLANG_TIDY ?= ON
CMAKE_BUILD_TYPE ?= Release
BITSEL_ROOT ?= $(shell pwd)
CMAKE_INSTALL_PREFIX ?= $(BITSEL_ROOT)/build/fakeinstall
CMAKE_ARGS ?=
VERBOSE ?=

ifeq ($(CMAKE_BUILD_TYPE), Debug)
	BUILD_PATH ?= build_dbg
else
	BUILD_PATH ?= build_dev
endif

.PHONY: default
default: build

.PHONY: clean
clean:
	make -C $(BUILD_PATH) clean

.PHONY: cmakeclean
cmakeclean:
	rm -rf $(BUILD_PATH)

.PHONY: cmake
cmake: $(BUILD_PATH)/Makefile
	cp $(BUILD_PATH)/compile_commands.json $(BITSEL_ROOT)

.PHONY: build
build: cmake
	make -C $(BUILD_PATH) VERBOSE=$(VERBOSE) bitsel

.PHONY: install
install: cmake
	make -C $(BUILD_PATH) VERBOSE=$(VERBOSE) install

.PHONY: test
test: cmake
	make -C $(BUILD_PATH) VERBOSE=$(VERBOSE) unit_tests
	$(BUILD_PATH)/test/unit_tests

.PHONY: example
example: example-des example-utils

.PHONY: example-des
example-des: cmake
	make -C $(BUILD_PATH) VERBOSE=$(VERBOSE) example-des

.PHONY: example-utils
example-utils: cmake
	make -C $(BUILD_PATH) VERBOSE=$(VERBOSE) example-utils

.PHONY: format
format: $(BUILD_PATH)/Makefile
	make -C $(BUILD_PATH) VERBOSE=$(VERBOSE) clang-format

$(BUILD_PATH)/Makefile: CMakeLists.txt Makefile
	mkdir -p $(BUILD_PATH) ; \
	cd $(BUILD_PATH) ; \
	env $(RUNENV) \
		cmake $(BITSEL_ROOT) \
		-DCMAKE_INSTALL_PREFIX=$(CMAKE_INSTALL_PREFIX) \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-DHIDE_SYMBOL=$(HIDE_SYMBOL) \
		-DDEBUG_SYMBOL=$(DEBUG_SYMBOL) \
		-DUSE_CLANG_TIDY=$(USE_CLANG_TIDY) \
		-DLINT_AS_ERRORS=ON \
		-DBITSEL_PROFILE=$(BITSEL_PROFILE) \
		$(CMAKE_ARGS)
