TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

SOURCES += main.cpp \
    ImageProcess/cvregion.cpp \
    ImageProcess/imageprocess.cpp


INCLUDEPATH += /usr/local/include \
               /usr/local/include/opencv \
               /usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so    \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_imgcodecs.so

HEADERS += \
    cvregion.h \
    ImageProcess/cvregion.h \
    ImageProcess/imageprocess.h
