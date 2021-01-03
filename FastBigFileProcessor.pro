TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lboost_system -lboost_filesystem -lboost_iostreams

SOURCES += \
        inputfilereader.cpp \
        main.cpp

HEADERS += \
    inputfilereader.hpp \
    threadsafequeue.hpp
