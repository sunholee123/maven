QT += widgets

TARGET = maven

CONFIG += c++11

LIBS += -lz
LIBS += -ldcmdata
LIBS += -lofstd

SOURCES += main.cpp \
    MainWindow.cpp \
    NiftiImage.cpp

HEADERS += \
    MainWindow.h \
    NiftiImage.h \
    nifti1.h

