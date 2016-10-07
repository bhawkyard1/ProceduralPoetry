TARGET = PoetryGenerator

OBJECTS_DIR = obj

isEqual(QT_MAJOR_VERSION, 5) {
        cache()
        #DEFINES  += QT5BUILD
}

INCLUDEPATH += $$PWD/include

MOC_DIR = moc

SOURCES += $$PWD/main.cpp \
                     $$PWD/src/*.cpp

HEADERS += $$PWD/include/*.hpp


OTHER_FILES += readme.md \
                           ./shaders/*.glsl

CONFIG += console
