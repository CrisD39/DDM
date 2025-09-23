QT += core
CONFIG += c++17 cmdline

SOURCES += \
    CommandDispatcher.cpp \
    main.cpp

HEADERS += \
    BaseCommand.h \
    CommandContext.h \
    CommandParser.h \
    CommandRegistry.h \
    EchoCommand.h \
    ICommand.h \
    IInputParser.h \
    CommandDispatcher.h
