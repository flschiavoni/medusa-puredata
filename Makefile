# pd-external Makefile

SRCDIR      := .
OBJDIR      := ./obj
EXTENSION   := pd_linux
DISTDIR     := ./pd
LINUXCFLAGS := -DPD -O2 -fPIC -funroll-loops -fomit-frame-pointer -Wall -W \
    -Wshadow -Wstrict-prototypes -Werror \
    -Wno-unused -Wno-parentheses -Wno-switch -lsctp -lpthread\
    -lc -lm -g
LDFLAGS     := -lsctp -lpthread -lopus -lmedusa -g
INCLUDE :=  -I.  -I./include/ -I/usr/local/include/medusa/

SOURCES := $(wildcard $(SRCDIR)/*.c)

current: $(OBJDIR) $(DISTDIR) all


$(OBJDIR):
	@ -mkdir $(OBJDIR)

$(DISTDIR):
	@ -mkdir $(DISTDIR)

all: $(addprefix $(DISTDIR)/, $(addsuffix .$(EXTENSION),$(basename $(notdir $(SOURCES)))))
	cp help/* $(DISTDIR)

$(DISTDIR)/%.$(EXTENSION): $(SRCDIR)/%.c
	cc $(LINUXCFLAGS) $(INCLUDE) -o $(OBJDIR)/$*.o -c $(SRCDIR)/$*.c
	ld -shared -o $(DISTDIR)/$*.pd_linux $(OBJDIR)/$*.o $(LDFLAGS)
	strip --strip-unneeded $(DISTDIR)/$*.pd_linux

clean: 
	(cd ../libmedusa && make clean)
	rm -rf $(OBJDIR)
	rm -rf $(DISTDIR)

install: all
	@ -mkdir -p ~/pd-externals/medusa
	cp pd/* ~/pd-externals/medusa/.

uninstall:
	rm -rf ~/pd-externals/medusa*

.PHONY: all clean install
