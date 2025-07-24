QT += widgets

TEMPLATE = lib
DEFINES += CANNYPLUGIN_LIBRARY

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../Algorithm_Platform_GUI/Plugins/MainInterface.cpp \
    ../../Algorithm_Platform_GUI/ViewWidget/CustomGraphicsView.cpp \
    ../../Algorithm_Platform_GUI/ViewWidget/CustomImageItem.cpp \
    ../../Algorithm_Platform_GUI/ViewWidget/CustomImageView.cpp \
    CannyPlugin.cpp \
    cannyui.cpp

HEADERS += \
    ../../Algorithm_Platform_GUI/Plugins/MainInterface.h \
    ../../Algorithm_Platform_GUI/ViewWidget/CustomGraphicsView.h \
    ../../Algorithm_Platform_GUI/ViewWidget/CustomImageItem.h \
    ../../Algorithm_Platform_GUI/ViewWidget/CustomImageView.h \
    ../../Algorithm_Platform_GUI/include/imageconvert.h \
    CannyPlugin_global.h \
    CannyPlugin.h \
    cannyui.h

INCLUDEPATH +=\
    $$PWD/../../Algorithm_Platform_GUI/Plugins\
    $$PWD/../../Algorithm_Platform_GUI/ViewWidget\
    $$PWD/../../Algorithm_Platform_GUI/include


#【5】配置opencv库
INCLUDEPATH += D:/programs/opencv4.10/build/include
Debug: {
LIBS += -lD:/programs/opencv4.10/build/x64/vc16/lib/opencv_world4100d
}
Release: {
LIBS += -lD:/programs/opencv4.10/build/x64/vc16/lib/opencv_world4100
}


#【6】配置输出路径，所有插件均生成至该路径
CONFIG(debug, debug|release){
    DESTDIR = $$PWD/../../../bin/plugins
}else{
    DESTDIR = $$PWD/../../../bin/plugins
}

#【7】配置边缘检测算法库
INCLUDEPATH += $$PWD/../../../include
#Debug: {
#LIBS += -l$$PWD/../../../bin/XL04_Canny
#}
#Release: {
#LIBS += -l$$PWD/../../../bin/XL04_Canny
#}
Debug: {
LIBS += -l$$PWD/../../../bin/CANNYDLL
}
Release: {
LIBS += -l$$PWD/../../../bin/CANNYDLL
}


# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    cannyui.ui
