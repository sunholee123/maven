QT += widgets

TARGET = maven

CONFIG += c++11

LIBS += -lz
LIBS += -ldcmdata
LIBS += -lofstd

SOURCES += main.cpp \
    MainWindow.cpp \
    NiftiImage.cpp \
	Image.cpp

HEADERS += \
    MainWindow.h \
    NiftiImage.h \
    nifti1.h \
    Image.h

#QMAKE_CXXFLAGS_RELEASE = -O3 -Ofast
QMAKE_CXXFLAGS_RELEASE = -O3 -m64 -Ofast -flto -march=native -funroll-loops
