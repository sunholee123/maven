QT += widgets

TARGET = maven

CONFIG += c++11

LIBS += -lz
LIBS += -ldcmdata
LIBS += -lofstd

SOURCES += main.cpp \
    MainWindow.cpp \
    NiftiImage.cpp \
	Image.cpp \
    LCModelData.cpp

HEADERS += \
    MainWindow.h \
    NiftiImage.h \
    nifti1.h \
    Image.h \
    LCModelData.h

QMAKE_CXXFLAGS_RELEASE = -Ofast -pipe -flto -march=native


