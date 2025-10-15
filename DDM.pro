QT += core network
CONFIG += console c++17
TEMPLATE = app
TARGET = DDM

INCLUDEPATH += Headers

SOURCES += \
    Headers/overlayhandler.cpp \
    Headers/spc.cpp \
    Sources/Encoderlpd.cpp \
    Sources/dclconccontroller.cpp \
    Sources/OBMHandler.cpp \
    Sources/main.cpp \
    Sources/fcdecodificator.cpp \
    Sources/CommandDispatcher.cpp \
    Sources/Commands/AddCommand.cpp \
    Sources/Commands/CenterCommand.cpp \
    Sources/Commands/DeleteCommand.cpp \
    Sources/Commands/ListCommand.cpp \
    Sources/clientsocket.cpp \
    Sources/configuration.cpp \
    Sources/stdinreader.cpp

HEADERS += \
    Headers/BaseCommand.h \
    Headers/Commands/IOBMHandler.h \
    Headers/EchoCommand.h \
    Headers/IDecodificator.h \
    Headers/OBMHandler.h \
    Headers/fcdecodificator.h \
    Headers/ICommand.h \
    Headers/CommandContext.h \
    Headers/CommandParser.h \
    Headers/CommandRegistry.h \
    Headers/Commands/AddCommand.h \
    Headers/Commands/CenterCommand.h \
    Headers/Commands/DeleteCommand.h \
    Headers/Commands/ListCommand.h \
    Headers/ConsoleUtils.h \
    Headers/Encoderlpd.h \
    Headers/IInputParser.h \
    Headers/ansi.h \
    Headers/clientsocket.h \
    Headers/commanddispatcher.h \
    Headers/configuration.h \
    Headers/overlayhandler.h \
    Headers/spc.h \
    Headers/stdinreader.h \
    Sources/dclconccontroller.h

RESOURCES += resources.qrc
