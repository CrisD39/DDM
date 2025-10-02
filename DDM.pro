QT += core \
    widgets network
CONFIG += console c++17
TEMPLATE = app
TARGET = DDM

SOURCES += \
    Sources/Encoderlpd.cpp \
    Sources/main.cpp \
    # Sources/fcdecodificator.cpp \
    Sources/CommandDispatcher.cpp \
    Sources/Commands/AddCommand.cpp \
    Sources/Commands/DeleteCommand.cpp \
    Sources/Commands/CenterCommand.cpp \
    Sources/Commands/ListCommand.cpp \
    clientsocket.cpp \
    configuration.cpp

HEADERS += \
    Headers/BaseCommand.h \
    Headers/EchoCommand.h \
    Headers/IDecodificator.h \
    # Headers/fcdecodificator.h \
    Headers/ICommand.h \
    Headers/CommandContext.h \
    Headers/Encoderlpd.h \
    clientsocket.h \
    Headers/CommandRegistry.h \
    Headers/IInputParser.h \
    Headers/CommandParser.h \
    Headers/ConsoleUtils.h \
    Headers/ansi.h \
    Headers/commanddispatcher.h \
    Headers/Commands/AddCommand.h \
    Headers/Commands/DeleteCommand.h \
    Headers/Commands/CenterCommand.h \
    Headers/Commands/ListCommand.h \
    configuration.h

RESOURCES += \
    resources.qrc

INCLUDEPATH += Headers                           
                                                          
