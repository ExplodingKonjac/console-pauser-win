CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra
PREFIX   ?= /usr/local

TARGET = build/console-pauser
SRC    = main.cpp

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(TARGET) $(DESTDIR)$(PREFIX)/bin/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
