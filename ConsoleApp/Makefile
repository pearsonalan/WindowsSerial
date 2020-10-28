TOOLSDIR="$(VCINSTALLDIR)Tools\msvc\$(VCTOOLSVERSION)\bin\Host$(VSCMD_ARG_HOST_ARCH)\$(VSCMD_ARG_TGT_ARCH)"
LINK=$(TOOLSDIR)\link.exe
CPP=$(TOOLSDIR)\cl.exe
PANDOC=pandoc.exe

LINKFLAGS=/nologo
CPPFLAGS=/nologo /EHsc /sdl /WX
DEFINES=
INCLUDES=

.obj.exe:
	$(LINK) $(LINKFLAGS) /OUT:$@ $<

.cpp.obj:
	$(CPP) $(CPPFLAGS) $(DEFINES) $(INCLUDES) /Fo:$@ /c $<

all: build docs

build: serial.exe

docs: README.html

serial.exe: serial.obj

serial.obj: serial.cpp

.md.html:
	$(PANDOC) -t html -s -o $@ $<

.SUFFIXES: .md

clean:
	del serial.obj serial.exe
	del README.html
