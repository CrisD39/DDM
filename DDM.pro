QT += core network \
    widgets
CONFIG += console c++17
TEMPLATE = app
TARGET = DDM

RESOURCES += resources.qrc

INCLUDEPATH += \
    src/ \
    src/controller/ \
    src/controller/commands/ \
    src/model/ \
    src/model/decoders/ \
    src/model/network/ \
    src/model/obm/ \
    src/model/overlays/ \
    src/model/utils/ \
    src/model/enums/ \
    src/view/ \


HEADERS += \
    src/controller/commandDispatcher.h \
    src/controller/commandRegistry.h \
    src/controller/commands/ICommand.h \
    src/controller/commands/addCommand.h \
    src/controller/commands/addCursor.h \
    src/controller/commands/centerCommand.h \
    src/controller/commands/deleteCommand.h \
    src/controller/commands/deletecursorscommand.h \
    src/controller/commands/estcommand.h \
    src/controller/commands/listCommand.h \
    src/controller/commands/listcursorscommand.h \
    src/controller/dclConcController.h \
    src/controller/jsoncommandhandler.h \
    src/controller/jsonresponsebuilder.h \
    src/controller/messagerouter.h \
    src/controller/overlayHandler.h \
    src/model/commandContext.h \
    src/model/decoders/concDecoder.h \
    src/model/decoders/iDecoder.h \
    src/model/decoders/lpdEncoder.h \
    src/model/entities/cursorEntity.h \
    src/model/entities/stationEntity.h \
    src/model/enums/enumsTrack.h \
    src/model/enums/enumsOwnShip.h \
    src/model/network/clientSocket.h \
    src/model/network/iTransport.h \
    src/model/network/localipcclient.h \
    src/model/network/transportFactory.h \
    src/model/network/udpClientAdapter.h \
    src/model/obm/iOBMHandler.h \
    src/model/obm/obmHandler.h \
    src/model/overlays/aaw.h \
    src/model/overlays/apc.h \
    src/model/overlays/asw.h \
    src/model/overlays/ew.h \
    src/model/overlays/heco.h \
    src/model/overlays/ops.h \
    src/model/overlays/spc.h \
    src/model/ownCursor/owncurs.h \
    src/model/overlays/linco.h \
    src/model/ownShip/ownship.h \
    src/model/qek.h \
    src/model/ownship.h \
    src/model/utils/configuration.h \
    src/model/entities/track.h \
    src/model/utils/consoleUtils.h \
    src/model/utils/geometry.h \
    src/view/CommandParser.h \
    src/view/ansi.h \
    src/view/iInputParser.h \
    src/view/stdinreader.h

SOURCES += \
    src/controller/commandDispatcher.cpp \
    src/controller/commands/addCommand.cpp \
    src/controller/commands/addCursor.cpp \
    src/controller/commands/centerCommand.cpp \
    src/controller/commands/deleteCommand.cpp \
    src/controller/commands/deletecursorscommand.cpp \
    src/controller/commands/estcommand.cpp \
    src/controller/commands/listCommand.cpp \
    src/controller/commands/listcursorscommand.cpp \
    src/controller/dclConcController.cpp \
    src/controller/jsoncommandhandler.cpp \
    src/controller/jsonresponsebuilder.cpp \
    src/controller/messagerouter.cpp \
    src/controller/overlayHandler.cpp \
    src/main.cpp \
    src/model/decoders/concDecoder.cpp \
    src/model/decoders/lpdEncoder.cpp \
    src/model/entities/cursorEntity.cpp \
    src/model/network/clientSocket.cpp \
    src/model/network/localipcclient.cpp \
    src/model/network/transportFactory.cpp \
    src/model/network/udpClientAdapter.cpp \
    src/model/obm/obmHandler.cpp \
    src/model/overlays/aaw.cpp \
    src/model/overlays/apc.cpp \
    src/model/overlays/asw.cpp \
    src/model/overlays/ew.cpp \
    src/model/overlays/heco.cpp \
    src/model/overlays/ops.cpp \
    src/model/overlays/spc.cpp \
    src/model/overlays/linco.cpp \
    src/model/entities/track.cpp \
    src/model/ownShip/ownship.cpp \
    src/model/ownCursor/owncurs.cpp \
    src/model/utils/configuration.cpp \
    src/view/stdinreader.cpp

