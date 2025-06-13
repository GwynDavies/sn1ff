# MIT License
# 
# Copyright (c) 2025 Gwyn Davies
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# .----------------------------------------------------------------.
# |                                                                |
# | Build with C code unit test coverage                           |
# |                                                                |
# '----------------------------------------------------------------'
#
# How to build and get test coverage
# COV=0
# #COV=1
#
# make clean
# make COVERAGE=$COV
# make COVERAGE=$COV test
# make clean; make COVERAGE=1; make COVERAGE=1 test 

# .----------------------------------------------------------------.
# |                                                                |
# | Project version variables                                      |
# |                                                                |
# '----------------------------------------------------------------'
#
# Define version as a variable
#
# There is a target to update this value in various files to match:
#     update-version
#
#R This is called from the 'all' target, so the version should always
# be made up-to-date
#
VERSION := 1.0
REVISION := 2
FULL_VERSION := $(VERSION)-$(REVISION)

# .----------------------------------------------------------------.
# |                                                                |
# | Package (amd64) variables                                      |
# |                                                                |
# '----------------------------------------------------------------'

DEBIAN_SERVER_PKG_DIR := ./sn1ff-server_$(FULL_VERSION)_amd64
DEBIAN_CLIENT_PKG_DIR := ./sn1ff-client_$(FULL_VERSION)_amd64
DEBIAN_SERVER_PKG_FILE := $(DEBIAN_SERVER_PKG_DIR).deb
DEBIAN_CLIENT_PKG_FILE := $(DEBIAN_CLIENT_PKG_DIR).deb

# .----------------------------------------------------------------.
# |                                                                |
# | Compilation variables                                          |
# |                                                                |
# '----------------------------------------------------------------'

# Compiler and flags
CC = gcc

# Define the default CFLAGS (for normal compile)
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic -g -I./include

# Define the CFLAGS for code-coverage
COVERAGE_CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic --coverage -g -I./include

# Check if we are doing code coverage testing
ifeq ($(COVERAGE), 1)
  CFLAGS = $(COVERAGE_CFLAGS) -DCOVERAGE=$(COVERAGE)
  $(info COVERAGE=1 - SELECT CODE COVERAGE INSTRUMENTATION)
else
  ifeq ($(COVERAGE), 0)
    $(info COVERAGE=0 - SELECT NO CODE COVERAGE INSTRUMENTION)
  else
    $(info COVERAGE NOT SET - DEFAULT TO NO CODE COVERAGE INTRUMENTATION)
  endif
endif

# .----------------------------------------------------------------.
# |                                                                |
# | Library variables                                              |
# |                                                                |
# '----------------------------------------------------------------'

LDFLAGS = -lncurses -luuid
TEST_LIBS = -lcriterion

# .----------------------------------------------------------------.
# |                                                                |
# | Project directory variables                                    |
# |                                                                |
# '----------------------------------------------------------------'

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests

# .----------------------------------------------------------------.
# |                                                                |
# | Target variables                                               |
# |                                                                |
# '----------------------------------------------------------------'

TARGETS = sn1ff_client sn1ff_service sn1ff_monitor sn1ff_cleaner sn1ff_license sn1ff_conf $(DEBIAN_SERVER_PKG_FILE) $(DEBIAN_CLIENT_PKG_FILE)

# Sources and objects for each target
CLIENT_SOURCES  = $(SRC_DIR)/sn1ff_client.c
SERVER_SOURCES  = $(SRC_DIR)/sn1ff_service.c
MONITOR_SOURCES = $(SRC_DIR)/sn1ff_monitor.c
CLEANER_SOURCES = $(SRC_DIR)/sn1ff_cleaner.c
LICENSE_SOURCES = $(SRC_DIR)/sn1ff_license.c
CONF_SOURCES    = $(SRC_DIR)/sn1ff_conf.c

CLIENT_OBJECTS  = $(OBJ_DIR)/sn1ff_client.o
SERVER_OBJECTS  = $(OBJ_DIR)/sn1ff_service.o
MONITOR_OBJECTS = $(OBJ_DIR)/sn1ff_monitor.o
CLEANER_OBJECTS = $(OBJ_DIR)/sn1ff_cleaner.o
LICENSE_OBJECTS = $(OBJ_DIR)/sn1ff_license.o
CONF_OBJECTS    = $(OBJ_DIR)/sn1ff_conf.o

OBJECTS = \
  $(OBJ_DIR)/cn_dir.o \
  $(OBJ_DIR)/cn_fpath.o \
  $(OBJ_DIR)/cn_file.o \
  $(OBJ_DIR)/cn_host.o \
  $(OBJ_DIR)/cn_multistr.o \
  $(OBJ_DIR)/cn_log.o \
  $(OBJ_DIR)/cn_net.o \
  $(OBJ_DIR)/cn_proc.o \
  $(OBJ_DIR)/cn_remotefe.o \
  $(OBJ_DIR)/cn_string.o \
  $(OBJ_DIR)/cn_time.o \
  $(OBJ_DIR)/sn_cfg.o \
  $(OBJ_DIR)/sn_cname.o \
  $(OBJ_DIR)/sn_dir.o \
  $(OBJ_DIR)/sn_file.o \
  $(OBJ_DIR)/sn_fname.o \
  $(OBJ_DIR)/sn_fpath.o \
  $(OBJ_DIR)/sn_status.o \
  $(OBJ_DIR)/sn_ui.o

# Test sources and objects
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)

# .----------------------------------------------------------------.
# |                                                                |
# | Targets for 'built artifacts'                                  |
# |                                                                |
# '----------------------------------------------------------------'

all: update-version $(TARGETS) $(DEBIAN_SERVER_PKG_FILE) $(DEBIAN_CLIENT_PKG_FILE)

update-version:
	@echo "Updating version in ./include/sn_ui.h to $(FULL_VERSION)"
	@sed -i 's/#define VERSION ".*"/#define VERSION "$(FULL_VERSION)"/' ./include/sn_const.h
	@echo "Updating version in ./debian/control-server to $(FULL_VERSION)"
	@sed -i 's/Version: .*/Version: $(FULL_VERSION)/' ./debian/control-server
	@echo "Updating version in ./debian/control-client to $(FULL_VERSION)"
	@sed -i 's/Version: .*/Version: $(FULL_VERSION)/' ./debian/control-client

sn1ff_client: $(CLIENT_OBJECTS) $(OBJECTS)
	#
	@echo "\n\nBuilding sn1ff client ...\n\n"
	#
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $(CLIENT_OBJECTS) $(OBJECTS) $(LDFLAGS)

sn1ff_service: $(SERVER_OBJECTS) $(OBJECTS)
	#
	@echo "\n\nBuilding sn1ff_service ...\n\n"
	#
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $(SERVER_OBJECTS) $(OBJECTS) $(LDFLAGS)

sn1ff_monitor: $(MONITOR_OBJECTS) $(OBJECTS)
	#
	@echo "\n\nBuilding sn1ff_monitor ...\n\n"
	#
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $(MONITOR_OBJECTS) $(OBJECTS) $(LDFLAGS)

sn1ff_cleaner: $(CLEANER_OBJECTS) $(OBJECTS)
	#
	@echo "\n\nBuilding sn1ff_cleaner ...\n\n"
	#
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $(CLEANER_OBJECTS) $(OBJECTS) $(LDFLAGS)

sn1ff_license: $(LICENSE_OBJECTS) $(OBJECTS)
	#
	@echo "\n\nBuilding sn1ff_license ...\n\n"
	#
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $(LICENSE_OBJECTS) $(OBJECTS) $(LDFLAGS)

sn1ff_conf: $(CONF_OBJECTS) $(OBJECTS)
	#
	@echo "\n\nBuilding sn1ff_conf ...\n\n"
	#
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $(CONF_OBJECTS) $(OBJECTS) $(LDFLAGS)

# Compile .c to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# .----------------------------------------------------------------.
# |                                                                |
# | Target for running C unit tests                                |
# |                                                                |
# '----------------------------------------------------------------'

test: $(TEST_OBJECTS) $(OBJECTS)
	#
	@echo "\n\nRunning tests ...\n\n"
	#
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_program $(TEST_OBJECTS) $(OBJECTS) $(LDFLAGS) $(TEST_LIBS)
	$(BIN_DIR)/test_program
	# If COVERAGE is set to 1, run additional coverage steps
	@if [ "$(COVERAGE)" = "1" ]; then \
		lcov --capture --directory ./obj --output-file coverage.info; \
		genhtml coverage.info --output-directory out; \
		echo "Code coverage report generated in 'out'"; \
		rm -f coverage.info *.gcda *.gcno; \
		xdg-open ./out/index.html || echo "Failed to open coverage report in browser"; \
	fi

# Compile .c to .o
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# .----------------------------------------------------------------.
# |                                                                |
# | Target 'clean'                                                 |
# |                                                                |
# '----------------------------------------------------------------'

clean: deb-server-clean deb-client-clean
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.gc*
	rm -f coverage.info
	rm -rf ./out
	find $(BIN_DIR)  -type f -executable -exec rm -f {} \;
	rm -f ./snff_files/*.snff

# .----------------------------------------------------------------.
# |                                                                |
# | Targets to prepare to build packages                           |
# |                                                                |
# '----------------------------------------------------------------'

deb-server-setup:
	sudo apt install build-essential devscripts debhelper
	# Runtime  dpkg -l libncurses6
	sudo apt install libcriterion-dev uuid-dev libncurses-dev

deb-client-setup:
	sudo apt install build-essential devscripts debhelper
	sudo apt install libcriterion-dev uuid-dev libncurses-dev

# .----------------------------------------------------------------.
# |                                                                |
# | Target to build package - 'server'                             |
# |                                                                |
# '----------------------------------------------------------------'

$(DEBIAN_SERVER_PKG_FILE): deb-server-clean deb-server-build

deb-server-clean:
	#
	@echo "\n\nCleaning previous build of Debian package 'server' ...\n\n"
	#
	rm -rf $(DEBIAN_SERVER_PKG_DIR)

deb-server-build:
	#
	@echo "\n\nBuilding Debian package 'server' ...\n\n"
	#
	rm -f $(DEBIAN_SERVER_PKG_DIR).deb
	rm -rf $(DEBIAN_SERVER_PKG_DIR)
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)
	#
	#
	@echo "1. DEB PKG updating dir /DEBIAN ..."
	#
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)/DEBIAN
	cp ./debian/control-server  $(DEBIAN_SERVER_PKG_DIR)/DEBIAN/control
	cp ./debian/postinst-server $(DEBIAN_SERVER_PKG_DIR)/DEBIAN/postinst
	cp ./debian/postrm-server $(DEBIAN_SERVER_PKG_DIR)/DEBIAN/postrm 
	cp ./debian/prerm-server $(DEBIAN_SERVER_PKG_DIR)/DEBIAN/prerm
	cp ./debian/conffiles-server $(DEBIAN_SERVER_PKG_DIR)/DEBIAN/conffiles
	sudo chmod +x $(DEBIAN_SERVER_PKG_DIR)/DEBIAN/postinst
	sudo chmod +x $(DEBIAN_SERVER_PKG_DIR)/DEBIAN/postrm
	sudo chmod +x $(DEBIAN_SERVER_PKG_DIR)/DEBIAN/prerm
	#
	#
	@echo "2. DEB PKG updating dir /etc/sn1ff ..."
	#
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)/etc/sn1ff
	cp ./install/sn1ff.conf  $(DEBIAN_SERVER_PKG_DIR)/etc/sn1ff
	#
	#
	@echo "3. DEB PKG updating dir /lib/systemd/system/sn1ff.service"
	#
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)/lib/systemd/system
	cp ./install/sn1ff.service  $(DEBIAN_SERVER_PKG_DIR)/lib/systemd/system
	#
	#
	@echo "4. DEB PKG updaing dir /usr/bin ..."
	#
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)/usr/bin
	cp $(BIN_DIR)/sn1ff_service $(DEBIAN_SERVER_PKG_DIR)/usr/bin/
	cp $(BIN_DIR)/sn1ff_monitor $(DEBIAN_SERVER_PKG_DIR)/usr/bin/
	cp $(BIN_DIR)/sn1ff_client $(DEBIAN_SERVER_PKG_DIR)/usr/bin/
	cp $(BIN_DIR)/sn1ff_cleaner $(DEBIAN_SERVER_PKG_DIR)/usr/bin/
	cp $(BIN_DIR)/sn1ff_license $(DEBIAN_SERVER_PKG_DIR)/usr/bin/
	cp $(BIN_DIR)/sn1ff_conf $(DEBIAN_SERVER_PKG_DIR)/usr/bin/
	strip --strip-unneeded $(DEBIAN_SERVER_PKG_DIR)/usr/bin/*
	sudo chmod +x $(DEBIAN_SERVER_PKG_DIR)/usr/bin/*
	#
	#
	@echo "5. DEB PKG updating dir /usr/share/doc ..."
	#
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)/usr/share/doc/sn1ff-server
	cp ./debian/copyright $(DEBIAN_SERVER_PKG_DIR)/usr/share/doc/sn1ff-server/copyright
	cp ./debian/changelog $(DEBIAN_SERVER_PKG_DIR)/usr/share/doc/sn1ff-server/changelog.Debian
	gzip -9 --no-name $(DEBIAN_SERVER_PKG_DIR)/usr/share/doc/sn1ff-server/changelog.Debian
	#
	#
	@echo "6. DEB PKG updating dir /usr/share/man ..."
	#
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1
	cp install/man/man1/sn1ff_monitor.1 $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1
	cp install/man/man1/sn1ff_client.1 $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1
	cp install/man/man1/sn1ff_license.1 $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1
	cp install/man/man1/sn1ff_conf.1 $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1
	gzip -9 --no-name $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1/sn1ff_monitor.1
	gzip -9 --no-name $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1/sn1ff_client.1
	gzip -9 --no-name $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1/sn1ff_license.1
	gzip -9 --no-name $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man1/sn1ff_conf.1
	#
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man7
	cp install/man/man7/sn1ff.7 $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man7
	gzip -9 --no-name $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man7/sn1ff.7
	#
	mkdir -p $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man8
	cp install/man/man8/sn1ff_service.8 $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man8
	cp install/man/man8/sn1ff_cleaner.8 $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man8
	gzip -9 --no-name $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man8/sn1ff_service.8
	gzip -9 --no-name $(DEBIAN_SERVER_PKG_DIR)/usr/share/man/man8/sn1ff_cleaner.8
	#
	#
	@echo "7. DEB PKG building ..."
	#
	dpkg-deb --build --root-owner-group $(DEBIAN_SERVER_PKG_DIR)

deb-server-verify:
	#
	@echo "\n\nVerifying Debian package 'server' ...\n\n"
	#
	#dpkg -c $(DEBIAN_SERVER_PKG_DIR).deb
	lintian -v $(DEBIAN_SERVER_PKG_DIR).deb

deb-server-install:
	#
	@echo "\n\nInstalling Debian package 'server' ...\n\n"
	#
	sudo apt remove -y sn1ff-server || true
	sudo dpkg -i $(DEBIAN_SERVER_PKG_DIR).deb

deb-server-uninstall:
	#
	@echo "\n\nRemoving package 'server' ...\n\n"
	#
	sudo apt remove sn1ff-server || true
	sudo dpkg --purge sn1ff-server || true

# .----------------------------------------------------------------.
# |                                                                |
# | Target to build package - 'client'                             |
# |                                                                |
# '----------------------------------------------------------------'

$(DEBIAN_CLIENT_PKG_FILE): deb-client-clean deb-client-build

deb-client-clean:
	#
	@echo "\n\nCleaning previous build of Debian package 'client' ...\n\n"
	#
	rm -rf $(DEBIAN_CLIENT_PKG_DIR)

deb-client-build:
	#
	@echo "\n\nBuilding Debian package 'client' ...\n\n"
	#
	rm -f $(DEBIAN_CLIENT_PKG_DIR).deb
	rm -rf $(DEBIAN_CLIENT_PKG_DIR)
	mkdir -p $(DEBIAN_CLIENT_PKG_DIR)
	#
	#
	@echo "1. DEB PKG updating dir /DEBIAN ..."
	#
	mkdir -p $(DEBIAN_CLIENT_PKG_DIR)/DEBIAN
	cp ./debian/control-client  $(DEBIAN_CLIENT_PKG_DIR)/DEBIAN/control
	cp ./debian/conffiles-client $(DEBIAN_CLIENT_PKG_DIR)/DEBIAN/conffiles
	#
	#
	@echo "2. DEB PKG updating dir /etc/sn1ff ..."
	#
	mkdir -p $(DEBIAN_CLIENT_PKG_DIR)/etc/sn1ff
	cp ./install/sn1ff.conf  $(DEBIAN_CLIENT_PKG_DIR)/etc/sn1ff
	#
	#
	@echo "3. DEB PKG updaing dir /usr/bin ..."
	#
	mkdir -p $(DEBIAN_CLIENT_PKG_DIR)/usr/bin
	cp $(BIN_DIR)/sn1ff_client $(DEBIAN_CLIENT_PKG_DIR)/usr/bin/
	cp $(BIN_DIR)/sn1ff_license $(DEBIAN_CLIENT_PKG_DIR)/usr/bin/
	cp $(BIN_DIR)/sn1ff_conf $(DEBIAN_CLIENT_PKG_DIR)/usr/bin/
	strip --strip-unneeded $(DEBIAN_CLIENT_PKG_DIR)/usr/bin/*
	sudo chmod +x $(DEBIAN_CLIENT_PKG_DIR)/usr/bin/*
	#
	#
	@echo "4. DEB PKG updating dir /usr/share/doc ..."
	#
	mkdir -p $(DEBIAN_CLIENT_PKG_DIR)/usr/share/doc/sn1ff-client
	cp ./debian/copyright $(DEBIAN_CLIENT_PKG_DIR)/usr/share/doc/sn1ff-client/copyright
	cp ./debian/changelog $(DEBIAN_CLIENT_PKG_DIR)/usr/share/doc/sn1ff-client/changelog.Debian
	gzip -9 --no-name $(DEBIAN_CLIENT_PKG_DIR)/usr/share/doc/sn1ff-client/changelog.Debian
	#
	#
	@echo "5. DEB PKG updating dir /usr/share/man ..."
	#
	mkdir -p $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1
	cp install/man/man1/sn1ff_monitor.1 $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1
	cp install/man/man1/sn1ff_client.1 $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1
	cp install/man/man1/sn1ff_license.1 $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1
	cp install/man/man1/sn1ff_conf.1 $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1
	gzip -9 --no-name $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1/sn1ff_monitor.1
	gzip -9 --no-name $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1/sn1ff_client.1
	gzip -9 --no-name $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1/sn1ff_license.1
	gzip -9 --no-name $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man1/sn1ff_conf.1
	#
	mkdir -p $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man7
	cp install/man/man7/sn1ff.7 $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man7
	gzip -9 --no-name $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man7/sn1ff.7
	#
	mkdir -p $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man8
	cp install/man/man8/sn1ff_service.8 $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man8
	cp install/man/man8/sn1ff_cleaner.8 $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man8
	gzip -9 --no-name $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man8/sn1ff_service.8
	gzip -9 --no-name $(DEBIAN_CLIENT_PKG_DIR)/usr/share/man/man8/sn1ff_cleaner.8
	#
	#
	@echo "6. DEB PKG building ..."
	#
	dpkg-deb --build --root-owner-group $(DEBIAN_CLIENT_PKG_DIR)

deb-client-verify:
	#
	@echo "\n\nVerifying Debian package 'client' ...\n\n"
	#
	#dpkg -c $(DEBIAN_CLIENT_PKG_DIR).deb
	lintian -v $(DEBIAN_CLIENT_PKG_DIR).deb

deb-client-install:
	#
	@echo "\n\nInstalling Debian package 'client' ...\n\n"
	#
	sudo apt remove -y sn1ff-client || true
	sudo dpkg -i $(DEBIAN_CLIENT_PKG_DIR).deb

deb-client-uninstall:
	#
	@echo "\n\nRemoving package 'client' ...\n\n"
	#
	sudo apt remove sn1ff-client || true
	sudo dpkg --purge sn1ff-client || true

# .----------------------------------------------------------------.
# |                                                                |
# | Targets for general package tasks                              |
# |                                                                |
# '----------------------------------------------------------------'

deb-verify: deb-server-verify deb-client-verify

# .----------------------------------------------------------------.
# |                                                                |
# | Target for 'help'                                              |
# |                                                                |
# '----------------------------------------------------------------'

help:
	clear
	echo "make clean"
	echo "make OR make COVERAGE=1"

# .----------------------------------------------------------------.
# |                                                                |
# | 'Phony' targets                                                |
# |                                                                |
# '----------------------------------------------------------------'

.PHONY: all clean test \
	deb-server-clean deb-server-setup deb-server-build deb-server-install deb-server-uninstall \
	deb-client-clean deb-client-setup deb-client-build deb-client-install deb-client-uninstall \
	deb-verify deb-client-verify deb-server-verify \
	update-version help

