TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lglfw -lGL -lGLEW -lm

SOURCES += main.c

HEADERS += \
	callbacks.h \
	uniforms.h \
	utils.h

OTHER_FILES += \
	float.frag

