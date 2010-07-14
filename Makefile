#############################################################################
# Makefile for building: dudley
# Generated by qmake (2.01a) (Qt 4.6.2) on: Mon Jun 21 16:48:15 2010
# Project:  dudley.pro
# Template: app
# Command: /home/rob/qtsdk-2010.02/qt/bin/qmake -spec ../../qtsdk-2010.02/qt/mkspecs/linux-g++-64 -unix CONFIG+=debug -o Makefile dudley.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I../../qtsdk-2010.02/qt/mkspecs/linux-g++-64 -I. -I../../qtsdk-2010.02/qt/include/QtCore -I../../qtsdk-2010.02/qt/include -I. -I.
LINK          = g++
LFLAGS        = -m64 -Wl,-rpath,/home/rob/qtsdk-2010.02/qt/lib
LIBS          = $(SUBLIBS)  -L/home/rob/qtsdk-2010.02/qt/lib -lQtCore -L/home/rob/qtsdk-2010.02/qt/lib -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /home/rob/qtsdk-2010.02/qt/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		fileinfo.cpp \
		filerepostate.cpp \
		filerepostatelogger.cpp \
		filerepo.cpp \
		workingfilerepo.cpp \
		storagefilerepo.cpp \
		output.cpp \
		filerepomanager.cpp 
OBJECTS       = main.o \
		fileinfo.o \
		filerepostate.o \
		filerepostatelogger.o \
		filerepo.o \
		workingfilerepo.o \
		storagefilerepo.o \
		output.o \
		filerepomanager.o
DIST          = ../../qtsdk-2010.02/qt/mkspecs/common/g++.conf \
		../../qtsdk-2010.02/qt/mkspecs/common/unix.conf \
		../../qtsdk-2010.02/qt/mkspecs/common/linux.conf \
		../../qtsdk-2010.02/qt/mkspecs/qconfig.pri \
		../../qtsdk-2010.02/qt/mkspecs/features/qt_functions.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/qt_config.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/exclusive_builds.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/default_pre.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/debug.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/default_post.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/warn_on.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/qt.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/unix/thread.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/moc.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/resources.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/uic.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/yacc.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/lex.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/include_source_dir.prf \
		dudley.pro
QMAKE_TARGET  = dudley
DESTDIR       = 
TARGET        = dudley

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_mainwindow.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: dudley.pro  ../../qtsdk-2010.02/qt/mkspecs/linux-g++-64/qmake.conf ../../qtsdk-2010.02/qt/mkspecs/common/g++.conf \
		../../qtsdk-2010.02/qt/mkspecs/common/unix.conf \
		../../qtsdk-2010.02/qt/mkspecs/common/linux.conf \
		../../qtsdk-2010.02/qt/mkspecs/qconfig.pri \
		../../qtsdk-2010.02/qt/mkspecs/features/qt_functions.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/qt_config.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/exclusive_builds.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/default_pre.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/debug.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/default_post.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/warn_on.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/qt.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/unix/thread.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/moc.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/resources.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/uic.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/yacc.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/lex.prf \
		../../qtsdk-2010.02/qt/mkspecs/features/include_source_dir.prf \
		/home/rob/qtsdk-2010.02/qt/lib/libQtCore.prl
	$(QMAKE) -spec ../../qtsdk-2010.02/qt/mkspecs/linux-g++-64 -unix CONFIG+=debug -o Makefile dudley.pro
../../qtsdk-2010.02/qt/mkspecs/common/g++.conf:
../../qtsdk-2010.02/qt/mkspecs/common/unix.conf:
../../qtsdk-2010.02/qt/mkspecs/common/linux.conf:
../../qtsdk-2010.02/qt/mkspecs/qconfig.pri:
../../qtsdk-2010.02/qt/mkspecs/features/qt_functions.prf:
../../qtsdk-2010.02/qt/mkspecs/features/qt_config.prf:
../../qtsdk-2010.02/qt/mkspecs/features/exclusive_builds.prf:
../../qtsdk-2010.02/qt/mkspecs/features/default_pre.prf:
../../qtsdk-2010.02/qt/mkspecs/features/debug.prf:
../../qtsdk-2010.02/qt/mkspecs/features/default_post.prf:
../../qtsdk-2010.02/qt/mkspecs/features/warn_on.prf:
../../qtsdk-2010.02/qt/mkspecs/features/qt.prf:
../../qtsdk-2010.02/qt/mkspecs/features/unix/thread.prf:
../../qtsdk-2010.02/qt/mkspecs/features/moc.prf:
../../qtsdk-2010.02/qt/mkspecs/features/resources.prf:
../../qtsdk-2010.02/qt/mkspecs/features/uic.prf:
../../qtsdk-2010.02/qt/mkspecs/features/yacc.prf:
../../qtsdk-2010.02/qt/mkspecs/features/lex.prf:
../../qtsdk-2010.02/qt/mkspecs/features/include_source_dir.prf:
/home/rob/qtsdk-2010.02/qt/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec ../../qtsdk-2010.02/qt/mkspecs/linux-g++-64 -unix CONFIG+=debug -o Makefile dudley.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/dudley1.0.0 || $(MKDIR) .tmp/dudley1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/dudley1.0.0/ && $(COPY_FILE) --parents fileinfo.h filerepostate.h filerepostatelogger.h filerepo.h workingfilerepo.h storagefilerepo.h output.h filerepomanager.h .tmp/dudley1.0.0/ && $(COPY_FILE) --parents main.cpp fileinfo.cpp filerepostate.cpp filerepostatelogger.cpp filerepo.cpp workingfilerepo.cpp storagefilerepo.cpp output.cpp filerepomanager.cpp .tmp/dudley1.0.0/ && $(COPY_FILE) --parents mainwindow.ui .tmp/dudley1.0.0/ && (cd `dirname .tmp/dudley1.0.0` && $(TAR) dudley1.0.0.tar dudley1.0.0 && $(COMPRESS) dudley1.0.0.tar) && $(MOVE) `dirname .tmp/dudley1.0.0`/dudley1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/dudley1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all:
compiler_moc_header_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_mainwindow.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainwindow.h
ui_mainwindow.h: mainwindow.ui
	/home/rob/qtsdk-2010.02/qt/bin/uic mainwindow.ui -o ui_mainwindow.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_uic_clean 

####### Compile

main.o: main.cpp filerepo.h \
		filerepostate.h \
		fileinfo.h \
		filerepostatelogger.h \
		workingfilerepo.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

fileinfo.o: fileinfo.cpp fileinfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o fileinfo.o fileinfo.cpp

filerepostate.o: filerepostate.cpp filerepostate.h \
		fileinfo.h \
		filerepostatelogger.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o filerepostate.o filerepostate.cpp

filerepostatelogger.o: filerepostatelogger.cpp filerepostatelogger.h \
		fileinfo.h \
		filerepostate.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o filerepostatelogger.o filerepostatelogger.cpp

filerepo.o: filerepo.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o filerepo.o filerepo.cpp

workingfilerepo.o: workingfilerepo.cpp workingfilerepo.h \
		filerepostatelogger.h \
		fileinfo.h \
		filerepostate.h \
		filerepo.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o workingfilerepo.o workingfilerepo.cpp

storagefilerepo.o: storagefilerepo.cpp storagefilerepo.h \
		filerepo.h \
		filerepostate.h \
		fileinfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o storagefilerepo.o storagefilerepo.cpp

output.o: output.cpp output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o output.o output.cpp

filerepomanager.o: filerepomanager.cpp filerepomanager.h \
		filerepo.h \
		filerepostate.h \
		fileinfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o filerepomanager.o filerepomanager.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

