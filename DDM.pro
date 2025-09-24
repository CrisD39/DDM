QT += core
CONFIG += c++17 cmdline

SOURCES += \
    CommandDispatcher.cpp \
    fcdecodificator.cpp \
    main.cpp

HEADERS += \
    BaseCommand.h \
    CommandContext.h \
    CommandParser.h \
    CommandRegistry.h \
    EchoCommand.h \
    ICommand.h \
    IDecodificator.h \
    IInputParser.h \
    CommandDispatcher.h \
    fcdecodificator.h

DISTFILES +=

RESOURCES += \
    resources.qrc
