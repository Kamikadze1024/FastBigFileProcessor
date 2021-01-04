TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread -lboost_system -lboost_filesystem -lboost_iostreams

SOURCES += \
        inputfilereader.cpp \
        main.cpp \
        task.cpp

HEADERS += \
    inputfilereader.hpp \
    task.hpp \
    threadpool.hpp \
    threadsafequeue.hpp
