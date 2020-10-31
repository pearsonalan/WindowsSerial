TOOLSDIR="$(VCINSTALLDIR)Tools\msvc\$(VCTOOLSVERSION)\bin\Host$(VSCMD_ARG_HOST_ARCH)\$(VSCMD_ARG_TGT_ARCH)"
LINK=$(TOOLSDIR)\link.exe
CPP=$(TOOLSDIR)\cl.exe
RC=rc.exe
PANDOC=pandoc.exe

LINKFLAGS=/nologo /SUBSYSTEM:windows
LIBS=user32.lib kernel32.lib gdi32.lib advapi32.lib
CPPFLAGS=/nologo /EHsc /sdl /WX /MDd
DEFINES=/D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE"
INCLUDES=

APP=monitor.exe
OBJS=winfx.obj MainWindow.obj MonitorApp.obj PreferencesDialog.obj Registry.obj Serial.obj
RES=monitor.res

HTML=README.html

all: build docs

build: $(APP)

docs: $(HTML) 

$(APP): $(OBJS) $(RES)
	$(LINK) $(LINKFLAGS) /OUT:$(APP) $(OBJS) $(RES) $(LIBS)

MainWindow.obj: MainWindow.cpp MainWindow.h PreferencesDialog.h framework.h winfx.h Resource.h Monitor.h
MonitorApp.obj: MonitorApp.cpp MainWindow.h framework.h winfx.h Resource.h
PreferencesDialog.obj: PreferencesDialog.cpp PreferencesDialog.h framework.h winfx.h Registry.h Resource.h Monitor.h
Registry.obj: Registry.cpp framework.h winfx.h Registry.h
Serial.obj: Serial.cpp framework.h winfx.h Serial.h
winfx.obj: winfx.cpp framework.h winfx.h

.cpp.obj:
	$(CPP) $(CPPFLAGS) $(DEFINES) $(INCLUDES) /Fo:$@ /c $<

.rc.res:
	$(RC) /fo $@ $<

.md.html:
	$(PANDOC) -t html -s -o $@ $<

.SUFFIXES: .md

clean:
	-del $(APP)
	-del $(OBJS)
	-del $(RES)
	-del $(HTML)