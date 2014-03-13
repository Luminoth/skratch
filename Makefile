# general variables
CXX = g++
VERSION = 0.1.1

# application variables
PROGNAME = skratch

# source directory variables
SRCDIR = src
INCDIR = include

# creatable directories
BINDIR = bin
OBJDIR = obj

# other directories
DATADIR = data

# compiler options
BASE_CFLAGS = -Wall -Woverloaded-virtual -pipe
#CFLAGS = $(BASE_CFLAGS) -DDEBUG -g -gstabs+ -fno-default-inline
#CFLAGS = $(BASE_CFLAGS) -DDEBUG -g -gstabs+ -fno-default-inline -pg
#CFLAGS = $(BASE_CFLAGS) -DNDEBUG -O3 -finline-functions -ffast-math -fno-common -funroll-loops -pg
CFLAGS = $(BASE_CFLAGS) -DNDEBUG -O3 -finline-functions -fomit-frame-pointer -ffast-math -fno-common -funroll-loops


# targets
all: $(BINDIR)
	$(MAKE) -C $(SRCDIR) CXX="$(CXX)" INCDIR="$(INCDIR)" OBJDIR="$(OBJDIR)" PROGNAME="$(PROGNAME)" CFLAGS="$(CFLAGS)"
	cp $(SRCDIR)/$(PROGNAME) $(BINDIR)

$(BINDIR):
	mkdir $(BINDIR)

strip: all
	strip -sv $(BINDIR)/*

clean:
	$(MAKE) -C $(SRCDIR) OBJDIR="$(OBJDIR)" PROGNAME="$(PROGNAME)" $@
	rm -rf $(BINDIR)

install: strip
	@echo "not done"

package: clean
	cp -R ../$(PROGNAME)/ ../$(PROGNAME)-$(VERSION)-src/
	mv ../$(PROGNAME)-$(VERSION)-src/ ./
	tar -cvf $(PROGNAME)-$(VERSION)-src.tar $(PROGNAME)-$(VERSION)-src/
	gzip $(PROGNAME)-$(VERSION)-src.tar
	rm -rf $(PROGNAME)-$(VERSION)-src/
	$(MAKE) all
	$(MAKE) -C $(SRCDIR) CXX="$(CXX)" PROGNAME="$(PROGNAME)" CFLAGS="$(CFLAGS)"
	tar -cvf $(PROGNAME)-$(VERSION).tar $(BINDIR) $(DATADIR)
	gzip $(PROGNAME)-$(VERSION).tar
	$(MAKE) clean
