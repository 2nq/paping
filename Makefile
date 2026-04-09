CXX      = g++
CXXFLAGS = -O2 -Wall
SRCDIR   = src
SRCS     = $(SRCDIR)/main.cpp $(SRCDIR)/arguments.cpp $(SRCDIR)/host.cpp \
           $(SRCDIR)/socket.cpp $(SRCDIR)/print.cpp $(SRCDIR)/stats.cpp \
           $(SRCDIR)/timer.cpp $(SRCDIR)/i18n.cpp $(SRCDIR)/gettimeofday.c
TARGET   = paping

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install clean
