
TARGET = transweb_external_client_dev
CONFIG += console
CONFIG -= app_bundle
QT -= gui
TEMPLATE = app


DEFINES += _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64 DEBUG TRANSCEND_GUI
INCLUDEPATH += ../../trans_include ../../global_include ../../zlib

INTERCOM_LIB_PATH=$$PWD/..


QMAKE_CXX=gcc

LIBS += $$INTERCOM_LIB_PATH/fake_transweb_client.o

## default use the same library Intercom.so, comment or set it emnpty to use static library from Intercom without Perl
SHARED_INTERCOM = 1

!isEmpty(SHARED_INTERCOM) {
  INTERCOM_LIB_PATH=$$INTERCOM_LIB_PATH/../RELEASE_DIR
  message(INTERCOM_LIB_PATH=$$INTERCOM_LIB_PATH)
  LIBS += -Wl,-rpath,$$INTERCOM_LIB_PATH  -L$$INTERCOM_LIB_PATH $$INTERCOM_LIB_PATH/Intercom.so
  DIR = /usr/lib64 /usr/lib
  PERLLIB=
  for(d, DIR) {
      PERL = libperl.a libperl.so
      for(l, PERL) {
            isEmpty(PERLLIB) {
               PERLLIB= $$system(find $${d} -name $${l} 2>/dev/null | head -1)
            }
      }
  }
  LIBS += $$PERLLIB
  message(using shared Intercom.so PER5LIB=$$PERLLIB)
}
else {
  LIBS += $$INTERCOM_LIB_PATH/../libexternsslintercom.a -lssl -lcrypto $$INTERCOM_LIB_PATH/../zlib/libz.a
  message(using static library $$INTERCOM_LIB_PATH/../libexternsslintercom.a)
}



#LIBS += -Wl,-rpath,/run/pkg/TWW-libopenssl-/1.0.0e/lib -L/run/pkg/TWW-libopenssl-/1.0.0e/lib -lssl -lcrypto
#LIBS += /run/pkg/TWW-libopenssl-/1.0.0e/lib/libssl.a /run/pkg/TWW-libopenssl-/1.0.0e/lib/libcrypto.a
#LIBS +=  -lssl -lcrypto
#LIBS += /home/carlos/Projects/trans/nxp/zlib/libz.a -lm -ldl
LIBS +=  -lm -ldl
SOURCES += main.c fake_perl_lib.c 

HEADERS += ../../trans_include/intercom_lib.h ../../trans_include/trans_defines.h
    
    
