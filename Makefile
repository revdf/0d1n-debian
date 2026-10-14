CC=gcc
CFLAGS=-W -Wall -Wextra -Wformat-security -Wno-maybe-uninitialized -O2
HARDENING=-fstack-protector-all -pie -fPIE
DIR=src/
DIROUT=bin/
DIR_HEADERS=src/headers/
UNAME_S := $(shell uname -s)
BIN=0d1n-debian
OPTDIR=/opt/0d1n-debian

ifeq ($(UNAME_S),Darwin)
	LDFLAGS=-Wl,-lcurl -lpthread
else
	LDFLAGS=-Wl,-z,relro,-z,now -lcurl -lpthread
endif

all: $(BIN)

$(BIN): $(DIR)0d1n.c
	mkdir -p $(DIROUT)
	$(CC) $(CFLAGS) $(HARDENING) -c $(DIR)*.c -I$(DIR_HEADERS)
	$(CC) $(HARDENING) -o $(DIROUT)$(BIN) *.o $(LDFLAGS)
	rm -f *.o

clean:
	rm -rf *.o $(DIROUT)$(BIN) $(BIN)

PREFIX=/usr/local
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(OPTDIR)/view/response_sources
	mkdir -p $(DESTDIR)$(OPTDIR)/payloads
	install -m 0755 bin/$(BIN) $(DESTDIR)$(PREFIX)/bin/$(BIN)
	cp -rf templates $(DESTDIR)$(OPTDIR)/ 2>/dev/null || true
	cp -rf tables $(DESTDIR)$(OPTDIR)/view/ 2>/dev/null || true
	cp -rf payloads $(DESTDIR)$(OPTDIR)/ 2>/dev/null || true
	chmod 0755 -R $(DESTDIR)$(OPTDIR)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)
	rm -rf $(DESTDIR)$(OPTDIR)
	rm -f *.o $(BIN)
