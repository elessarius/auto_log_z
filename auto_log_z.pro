TEMPLATE = app
CONFIG -= console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    third_party/zip.c \
    functions.c \
    main.c

HEADERS += \
    functions.h \
    third_party/miniz.h \
    third_party/zip.h
