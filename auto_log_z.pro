TEMPLATE = app
CONFIG -= console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    functions.cpp \
    main.cpp \
    third_party/zip.c

HEADERS += \
    functions.h \
    third_party/miniz.h \
    third_party/zip.h
