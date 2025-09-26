
QT += core
CONFIG += console c++17
TEMPLATE = app
TARGET = DDM

INCLUDEPATH += Headers

SOURCES +=     Sources/main.cpp     Sources/CommandDispatcher.cpp     Sources/Commands/AddCommand.cpp     Sources/Commands/DeleteCommand.cpp     Sources/Commands/CenterCommand.cpp     Sources/Commands/ListCommand.cpp

HEADERS +=     Headers/ICommand.h     Headers/CommandContext.h     Headers/CommandRegistry.h     Headers/IInputParser.h     Headers/CommandParser.h     Headers/ConsoleUtils.h     Headers/ansi.h     Headers/commanddispatcher.h     Headers/Commands/AddCommand.h     Headers/Commands/DeleteCommand.h     Headers/Commands/CenterCommand.h     Headers/Commands/ListCommand.h
