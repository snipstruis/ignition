TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lglfw -lSOIL -lGL -lGLEW -lm

SOURCES += main.c

HEADERS += \
	callbacks.h \
	uniforms.h \
	utils.h \
    transform.h

OTHER_FILES += \
	float.frag \
    information.xcf \
    introduction.xcf \
    information.png \
    introduction.png \
    fullscreen.png

