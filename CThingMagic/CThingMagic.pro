TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Application/sargascpp_sample.c \
        libSargas/libSargasC.c

HEADERS += \
    Application/sargascpp_sample.h \
    libSargas/libSargasC.h
