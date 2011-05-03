#############################################################################
# Makefile for building: dudley
# Generated by qmake (2.01a) (Qt 4.7.3) on: Thu Apr 28 22:12:25 2011
# Project:  dudley.pro
# Template: app
# Command: /Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/qmake -o Makefile dudley.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -g -gdwarf-2 -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5 -Wall -W $(DEFINES)
CXXFLAGS      = -pipe -g -gdwarf-2 -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5 -Wall -W $(DEFINES)
INCPATH       = -I../../QtSDK/Desktop/Qt/473/gcc/mkspecs/default -I. -I../../QtSDK/Desktop/Qt/473/gcc/lib/QtCore.framework/Versions/4/Headers -I../../QtSDK/Desktop/Qt/473/gcc/include/QtCore -I../../QtSDK/Desktop/Qt/473/gcc/lib/QtNetwork.framework/Versions/4/Headers -I../../QtSDK/Desktop/Qt/473/gcc/include/QtNetwork -I../../QtSDK/Desktop/Qt/473/gcc/lib/QtGui.framework/Versions/4/Headers -I../../QtSDK/Desktop/Qt/473/gcc/include/QtGui -I../../QtSDK/Desktop/Qt/473/gcc/include -I../qjson/src -I. -I. -F/Users/rob/QtSDK/Desktop/Qt/473/gcc/lib
LINK          = g++
LFLAGS        = -headerpad_max_install_names -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
LIBS          = $(SUBLIBS) -F/Users/rob/QtSDK/Desktop/Qt/473/gcc/lib -L/Users/rob/QtSDK/Desktop/Qt/473/gcc/lib -L/Users/rob/src/dudley/../qjson/build/lib/ -lqjson -framework QtGui -framework QtNetwork -framework QtCore 
AR            = ar cq
RANLIB        = ranlib -s
QMAKE         = /Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
STRIP         = 
INSTALL_FILE  = $(COPY_FILE)
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = $(COPY_FILE)
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
export MACOSX_DEPLOYMENT_TARGET = 10.4

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		fileinfo.cpp \
		state.cpp \
		statelogger.cpp \
		localdiskrepo.cpp \
		output.cpp \
		httpserver.cpp \
		mainwindow.cpp \
		repodialog.cpp \
		repomodel.cpp \
		httprequest.cpp \
		httpresponse.cpp \
		httpclientrepo.cpp \
		mimetypefinder.cpp \
		filetransfer.cpp \
		filetransfermanager.cpp \
		httpmessage.cpp \
		repo.cpp \
		httpcontroller.cpp \
		httpview.cpp \
		stateop.cpp \
		statediff.cpp \
		synchroniser.cpp moc_state.cpp \
		moc_statelogger.cpp \
		moc_localdiskrepo.cpp \
		moc_output.cpp \
		moc_httpserver.cpp \
		moc_mainwindow.cpp \
		moc_repodialog.cpp \
		moc_repomodel.cpp \
		moc_httprequest.cpp \
		moc_httpresponse.cpp \
		moc_httpclientrepo.cpp \
		moc_filetransfer.cpp \
		moc_filetransfermanager.cpp \
		moc_httpmessage.cpp \
		moc_repo.cpp \
		moc_readoncebuffer.cpp \
		moc_httpcontroller.cpp \
		moc_synchroniser.cpp \
		qrc_icon.cpp
OBJECTS       = main.o \
		fileinfo.o \
		state.o \
		statelogger.o \
		localdiskrepo.o \
		output.o \
		httpserver.o \
		mainwindow.o \
		repodialog.o \
		repomodel.o \
		httprequest.o \
		httpresponse.o \
		httpclientrepo.o \
		mimetypefinder.o \
		filetransfer.o \
		filetransfermanager.o \
		httpmessage.o \
		repo.o \
		httpcontroller.o \
		httpview.o \
		stateop.o \
		statediff.o \
		synchroniser.o \
		moc_state.o \
		moc_statelogger.o \
		moc_localdiskrepo.o \
		moc_output.o \
		moc_httpserver.o \
		moc_mainwindow.o \
		moc_repodialog.o \
		moc_repomodel.o \
		moc_httprequest.o \
		moc_httpresponse.o \
		moc_httpclientrepo.o \
		moc_filetransfer.o \
		moc_filetransfermanager.o \
		moc_httpmessage.o \
		moc_repo.o \
		moc_readoncebuffer.o \
		moc_httpcontroller.o \
		moc_synchroniser.o \
		qrc_icon.o
DIST          = ../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/unix.conf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/mac.conf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/mac-g++.conf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/qconfig.pri \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/modules/qt_webkit_version.pri \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt_functions.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt_config.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/exclusive_builds.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/default_pre.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/default_pre.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/dwarf2.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/debug.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/default_post.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/default_post.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/x86_64.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/objective_c.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/warn_on.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/unix/thread.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/moc.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/rez.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/sdk.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/resources.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/uic.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/yacc.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/lex.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/include_source_dir.prf \
		dudley.pro
QMAKE_TARGET  = dudley
DESTDIR       = 
TARGET        = dudley

####### Custom Compiler Variables
QMAKE_COMP_QMAKE_OBJECTIVE_CFLAGS = -pipe \
		-g \
		-gdwarf-2 \
		-arch \
		x86_64 \
		-Xarch_x86_64 \
		-mmacosx-version-min=10.5 \
		-Wall \
		-W


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

$(TARGET): ui_mainwindow.h ui_repodialog.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: dudley.pro  ../../QtSDK/Desktop/Qt/473/gcc/mkspecs/default/qmake.conf ../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/unix.conf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/mac.conf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/mac-g++.conf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/qconfig.pri \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/modules/qt_webkit_version.pri \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt_functions.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt_config.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/exclusive_builds.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/default_pre.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/default_pre.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/dwarf2.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/debug.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/default_post.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/default_post.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/x86_64.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/objective_c.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/warn_on.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/unix/thread.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/moc.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/rez.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/sdk.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/resources.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/uic.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/yacc.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/lex.prf \
		../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/include_source_dir.prf
	$(QMAKE) -o Makefile dudley.pro
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/unix.conf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/mac.conf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/common/mac-g++.conf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/qconfig.pri:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/modules/qt_webkit_version.pri:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt_functions.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt_config.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/exclusive_builds.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/default_pre.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/default_pre.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/dwarf2.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/debug.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/default_post.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/default_post.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/x86_64.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/objective_c.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/warn_on.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/qt.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/unix/thread.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/moc.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/rez.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/mac/sdk.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/resources.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/uic.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/yacc.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/lex.prf:
../../QtSDK/Desktop/Qt/473/gcc/mkspecs/features/include_source_dir.prf:
qmake:  FORCE
	@$(QMAKE) -o Makefile dudley.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/dudley1.0.0 || $(MKDIR) .tmp/dudley1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/dudley1.0.0/ && $(COPY_FILE) --parents fileinfo.h state.h statelogger.h localdiskrepo.h output.h httpserver.h mainwindow.h repodialog.h repomodel.h httprequest.h httpresponse.h httpclientrepo.h mimetypefinder.h filetransfer.h filetransfermanager.h httpmessage.h repo.h readoncebuffer.h httpcontroller.h httpview.h stateop.h statediff.h synchroniser.h .tmp/dudley1.0.0/ && $(COPY_FILE) --parents icon.qrc .tmp/dudley1.0.0/ && $(COPY_FILE) --parents main.cpp fileinfo.cpp state.cpp statelogger.cpp localdiskrepo.cpp output.cpp httpserver.cpp mainwindow.cpp repodialog.cpp repomodel.cpp httprequest.cpp httpresponse.cpp httpclientrepo.cpp mimetypefinder.cpp filetransfer.cpp filetransfermanager.cpp httpmessage.cpp repo.cpp httpcontroller.cpp httpview.cpp stateop.cpp statediff.cpp synchroniser.cpp .tmp/dudley1.0.0/ && $(COPY_FILE) --parents mainwindow.ui repodialog.ui .tmp/dudley1.0.0/ && (cd `dirname .tmp/dudley1.0.0` && $(TAR) dudley1.0.0.tar dudley1.0.0 && $(COMPRESS) dudley1.0.0.tar) && $(MOVE) `dirname .tmp/dudley1.0.0`/dudley1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/dudley1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_moc_header_make_all: moc_state.cpp moc_statelogger.cpp moc_localdiskrepo.cpp moc_output.cpp moc_httpserver.cpp moc_mainwindow.cpp moc_repodialog.cpp moc_repomodel.cpp moc_httprequest.cpp moc_httpresponse.cpp moc_httpclientrepo.cpp moc_filetransfer.cpp moc_filetransfermanager.cpp moc_httpmessage.cpp moc_repo.cpp moc_readoncebuffer.cpp moc_httpcontroller.cpp moc_synchroniser.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_state.cpp moc_statelogger.cpp moc_localdiskrepo.cpp moc_output.cpp moc_httpserver.cpp moc_mainwindow.cpp moc_repodialog.cpp moc_repomodel.cpp moc_httprequest.cpp moc_httpresponse.cpp moc_httpclientrepo.cpp moc_filetransfer.cpp moc_filetransfermanager.cpp moc_httpmessage.cpp moc_repo.cpp moc_readoncebuffer.cpp moc_httpcontroller.cpp moc_synchroniser.cpp
moc_state.cpp: fileinfo.h \
		mimetypefinder.h \
		state.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ state.h -o moc_state.cpp

moc_statelogger.cpp: fileinfo.h \
		mimetypefinder.h \
		state.h \
		stateop.h \
		statediff.h \
		statelogger.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ statelogger.h -o moc_statelogger.cpp

moc_localdiskrepo.cpp: statelogger.h \
		fileinfo.h \
		mimetypefinder.h \
		state.h \
		stateop.h \
		statediff.h \
		repo.h \
		localdiskrepo.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ localdiskrepo.h -o moc_localdiskrepo.cpp

moc_output.cpp: output.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ output.h -o moc_output.cpp

moc_httpserver.cpp: fileinfo.h \
		mimetypefinder.h \
		httprequest.h \
		httpmessage.h \
		readoncebuffer.h \
		httpcontroller.h \
		httpresponse.h \
		repo.h \
		state.h \
		repomodel.h \
		filetransfermanager.h \
		filetransfer.h \
		httpserver.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ httpserver.h -o moc_httpserver.cpp

moc_mainwindow.cpp: httpserver.h \
		fileinfo.h \
		mimetypefinder.h \
		httprequest.h \
		httpmessage.h \
		readoncebuffer.h \
		httpcontroller.h \
		httpresponse.h \
		repo.h \
		state.h \
		repomodel.h \
		filetransfermanager.h \
		filetransfer.h \
		repodialog.h \
		output.h \
		mainwindow.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ mainwindow.h -o moc_mainwindow.cpp

moc_repodialog.cpp: repodialog.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ repodialog.h -o moc_repodialog.cpp

moc_repomodel.cpp: repo.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h \
		repomodel.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ repomodel.h -o moc_repomodel.cpp

moc_httprequest.cpp: httpmessage.h \
		readoncebuffer.h \
		httprequest.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ httprequest.h -o moc_httprequest.cpp

moc_httpresponse.cpp: httpmessage.h \
		readoncebuffer.h \
		httpresponse.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ httpresponse.h -o moc_httpresponse.cpp

moc_httpclientrepo.cpp: state.h \
		fileinfo.h \
		mimetypefinder.h \
		repo.h \
		httpclientrepo.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ httpclientrepo.h -o moc_httpclientrepo.cpp

moc_filetransfer.cpp: fileinfo.h \
		mimetypefinder.h \
		filetransfer.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ filetransfer.h -o moc_filetransfer.cpp

moc_filetransfermanager.cpp: filetransfer.h \
		fileinfo.h \
		mimetypefinder.h \
		filetransfermanager.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ filetransfermanager.h -o moc_filetransfermanager.cpp

moc_httpmessage.cpp: readoncebuffer.h \
		httpmessage.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ httpmessage.h -o moc_httpmessage.cpp

moc_repo.cpp: state.h \
		fileinfo.h \
		mimetypefinder.h \
		repo.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ repo.h -o moc_repo.cpp

moc_readoncebuffer.cpp: readoncebuffer.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ readoncebuffer.h -o moc_readoncebuffer.cpp

moc_httpcontroller.cpp: httprequest.h \
		httpmessage.h \
		readoncebuffer.h \
		httpresponse.h \
		repo.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h \
		repomodel.h \
		filetransfermanager.h \
		filetransfer.h \
		httpcontroller.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ httpcontroller.h -o moc_httpcontroller.cpp

moc_synchroniser.cpp: statediff.h \
		stateop.h \
		synchroniser.h
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ synchroniser.h -o moc_synchroniser.cpp

compiler_rcc_make_all: qrc_icon.cpp
compiler_rcc_clean:
	-$(DEL_FILE) qrc_icon.cpp
qrc_icon.cpp: icon.qrc \
		dino1.png \
		uploadform.html \
		mimetypes.txt
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/rcc -name icon icon.qrc -o qrc_icon.cpp

compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_uic_make_all: ui_mainwindow.h ui_repodialog.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainwindow.h ui_repodialog.h
ui_mainwindow.h: mainwindow.ui
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/uic mainwindow.ui -o ui_mainwindow.h

ui_repodialog.h: repodialog.ui
	/Users/rob/QtSDK/Desktop/Qt/473/gcc/bin/uic repodialog.ui -o ui_repodialog.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean compiler_uic_clean 

####### Compile

main.o: main.cpp repo.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h \
		statelogger.h \
		stateop.h \
		statediff.h \
		localdiskrepo.h \
		output.h \
		mainwindow.h \
		httpserver.h \
		httprequest.h \
		httpmessage.h \
		readoncebuffer.h \
		httpcontroller.h \
		httpresponse.h \
		repomodel.h \
		filetransfermanager.h \
		filetransfer.h \
		repodialog.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

fileinfo.o: fileinfo.cpp fileinfo.h \
		mimetypefinder.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o fileinfo.o fileinfo.cpp

state.o: state.cpp state.h \
		fileinfo.h \
		mimetypefinder.h \
		statelogger.h \
		stateop.h \
		statediff.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o state.o state.cpp

statelogger.o: statelogger.cpp statelogger.h \
		fileinfo.h \
		mimetypefinder.h \
		state.h \
		stateop.h \
		statediff.h \
		../qjson/src/qobjecthelper.h \
		../qjson/src/qjson_export.h \
		../qjson/src/serializer.h \
		../qjson/src/parser.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o statelogger.o statelogger.cpp

localdiskrepo.o: localdiskrepo.cpp repo.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h \
		localdiskrepo.h \
		statelogger.h \
		stateop.h \
		statediff.h \
		output.h \
		filetransfer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o localdiskrepo.o localdiskrepo.cpp

output.o: output.cpp output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o output.o output.cpp

httpserver.o: httpserver.cpp output.h \
		httpserver.h \
		fileinfo.h \
		mimetypefinder.h \
		httprequest.h \
		httpmessage.h \
		readoncebuffer.h \
		httpcontroller.h \
		httpresponse.h \
		repo.h \
		state.h \
		repomodel.h \
		filetransfermanager.h \
		filetransfer.h \
		statelogger.h \
		stateop.h \
		statediff.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o httpserver.o httpserver.cpp

mainwindow.o: mainwindow.cpp mainwindow.h \
		httpserver.h \
		fileinfo.h \
		mimetypefinder.h \
		httprequest.h \
		httpmessage.h \
		readoncebuffer.h \
		httpcontroller.h \
		httpresponse.h \
		repo.h \
		state.h \
		repomodel.h \
		filetransfermanager.h \
		filetransfer.h \
		repodialog.h \
		output.h \
		ui_mainwindow.h \
		localdiskrepo.h \
		statelogger.h \
		stateop.h \
		statediff.h \
		httpclientrepo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainwindow.o mainwindow.cpp

repodialog.o: repodialog.cpp repodialog.h \
		ui_repodialog.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o repodialog.o repodialog.cpp

repomodel.o: repomodel.cpp repomodel.h \
		repo.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o repomodel.o repomodel.cpp

httprequest.o: httprequest.cpp httprequest.h \
		httpmessage.h \
		readoncebuffer.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o httprequest.o httprequest.cpp

httpresponse.o: httpresponse.cpp httpresponse.h \
		httpmessage.h \
		readoncebuffer.h \
		output.h \
		fileinfo.h \
		mimetypefinder.h \
		repo.h \
		state.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o httpresponse.o httpresponse.cpp

httpclientrepo.o: httpclientrepo.cpp httpclientrepo.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h \
		repo.h \
		output.h \
		statelogger.h \
		stateop.h \
		statediff.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o httpclientrepo.o httpclientrepo.cpp

mimetypefinder.o: mimetypefinder.cpp mimetypefinder.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mimetypefinder.o mimetypefinder.cpp

filetransfer.o: filetransfer.cpp filetransfer.h \
		fileinfo.h \
		mimetypefinder.h \
		repo.h \
		state.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o filetransfer.o filetransfer.cpp

filetransfermanager.o: filetransfermanager.cpp filetransfermanager.h \
		filetransfer.h \
		fileinfo.h \
		mimetypefinder.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o filetransfermanager.o filetransfermanager.cpp

httpmessage.o: httpmessage.cpp httpmessage.h \
		readoncebuffer.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o httpmessage.o httpmessage.cpp

repo.o: repo.cpp repo.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h \
		statelogger.h \
		stateop.h \
		statediff.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o repo.o repo.cpp

httpcontroller.o: httpcontroller.cpp httpcontroller.h \
		httprequest.h \
		httpmessage.h \
		readoncebuffer.h \
		httpresponse.h \
		repo.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h \
		repomodel.h \
		filetransfermanager.h \
		filetransfer.h \
		statelogger.h \
		stateop.h \
		statediff.h \
		output.h \
		httpview.h \
		httpserver.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o httpcontroller.o httpcontroller.cpp

httpview.o: httpview.cpp httpview.h \
		fileinfo.h \
		mimetypefinder.h \
		output.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o httpview.o httpview.cpp

stateop.o: stateop.cpp stateop.h \
		state.h \
		fileinfo.h \
		mimetypefinder.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o stateop.o stateop.cpp

statediff.o: statediff.cpp statediff.h \
		stateop.h \
		../qjson/src/parser.h \
		../qjson/src/qjson_export.h \
		../qjson/src/json_scanner.h \
		../qjson/src/parser_p.h \
		../qjson/src/json_parser.hh \
		../qjson/src/stack.hh \
		../qjson/src/qjson_debug.h \
		../qjson/src/location.hh \
		../qjson/src/position.hh \
		../qjson/src/serializer.h \
		../qjson/src/qobjecthelper.h \
		fileinfo.h \
		mimetypefinder.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o statediff.o statediff.cpp

synchroniser.o: synchroniser.cpp synchroniser.h \
		statediff.h \
		stateop.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o synchroniser.o synchroniser.cpp

moc_state.o: moc_state.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_state.o moc_state.cpp

moc_statelogger.o: moc_statelogger.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_statelogger.o moc_statelogger.cpp

moc_localdiskrepo.o: moc_localdiskrepo.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_localdiskrepo.o moc_localdiskrepo.cpp

moc_output.o: moc_output.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_output.o moc_output.cpp

moc_httpserver.o: moc_httpserver.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_httpserver.o moc_httpserver.cpp

moc_mainwindow.o: moc_mainwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainwindow.o moc_mainwindow.cpp

moc_repodialog.o: moc_repodialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_repodialog.o moc_repodialog.cpp

moc_repomodel.o: moc_repomodel.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_repomodel.o moc_repomodel.cpp

moc_httprequest.o: moc_httprequest.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_httprequest.o moc_httprequest.cpp

moc_httpresponse.o: moc_httpresponse.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_httpresponse.o moc_httpresponse.cpp

moc_httpclientrepo.o: moc_httpclientrepo.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_httpclientrepo.o moc_httpclientrepo.cpp

moc_filetransfer.o: moc_filetransfer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_filetransfer.o moc_filetransfer.cpp

moc_filetransfermanager.o: moc_filetransfermanager.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_filetransfermanager.o moc_filetransfermanager.cpp

moc_httpmessage.o: moc_httpmessage.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_httpmessage.o moc_httpmessage.cpp

moc_repo.o: moc_repo.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_repo.o moc_repo.cpp

moc_readoncebuffer.o: moc_readoncebuffer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_readoncebuffer.o moc_readoncebuffer.cpp

moc_httpcontroller.o: moc_httpcontroller.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_httpcontroller.o moc_httpcontroller.cpp

moc_synchroniser.o: moc_synchroniser.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_synchroniser.o moc_synchroniser.cpp

qrc_icon.o: qrc_icon.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qrc_icon.o qrc_icon.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

