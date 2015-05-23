#-------------------------------------------------
#
# Project created by QtCreator 2012-06-22T07:45:38
#
#-------------------------------------------------

QT       += core gui xml

TARGET = LearnChess
TEMPLATE = app

CONFIG(debug, debug|release) {
    DEFINES += _DEBUG
}


#BOOST pas besoin des librairies car headers on
win32 {
INCLUDEPATH += D:/boost_1_52_0
#INCLUDEPATH += D:/Gecode/FANN-2.2.0-Source/FANN-2.2.0-Source/src/include
#LIBS += -LD:/Gecode/FANN-2.2.0-Source/FANN-2.2.0-Source/bin -lfannfloat
RC_FILE=Icone.rc
}
!win32 {
INCLUDEPATH += /home/alexis/BOOST/include
#INCLUDEPATH += /home/alexis/FANN/include
#LIBS += -L/home/alexis/FANN/lib -lfann
}

SOURCES += main.cpp\
        mainwindow.cpp \
    chessboardview.cpp \
    Data/tablepiece.cpp \
    frameinitiation.cpp \
    chessboardscene.cpp \
    ChessBoardManager/chessmanager.cpp \
    qgraphicspieceitem.cpp \
    infosgraphiques.cpp \
    descriptordemo.cpp \
    framecreationechiquier.cpp \
    qlistepieces.cpp \
    ChessBoardManager/IRuleMovePiece.cpp \
    ChessBoardManager/RuleMoveRook.cpp \
    ChessBoardManager/FactoryRuleMovePiece.cpp \
    ChessBoardManager/RuleMoveKnight.cpp \
    ChessBoardManager/RuleMoveBishop.cpp \
    ChessBoardManager/RuleMoveQueen.cpp \
    ChessBoardManager/RuleMoveKing.cpp \
    ChessBoardManager/RuleMovePawn.cpp \
    xmlreaderwriterfordemos.cpp \
    framecreationdemosmove.cpp \
    infosconfiguration.cpp \
    implinitiationdemomove.cpp \
    implinitiationtrainingmove.cpp \
    decochessboardviewwithrotation.cpp \
    ChessBoardManager/dialogaskpiece.cpp \
    ChessBoardManager/commandplay.cpp \
    ImportExport/pgnreaderwriter.cpp \
    TypePiece.cpp \
    ClientGraphique/framevisualizegames.cpp \
    ClientGraphique/framemultigames.cpp \
    ClientGraphique/dialogaskentetefichierpgn.cpp \
    ClientGraphique/affichages.cpp \
    Demos/actionsdemo.cpp \
    ClientGraphique/painterpathmoves.cpp \
    ClientGraphique/dialoginfosmove.cpp \
    ClientGraphique/dialogexplanation.cpp \
    ClientGraphique/frameintroduction.cpp \
    Demos/IFournisseurDemo.cpp \
    Demos/fournisseurdemomovepiece.cpp \
    Demos/factoryfournisseurdemo.cpp \
    Demos/fournisseurdemospecialmoves.cpp \
    Demos/fournisseurdemopatternsmat.cpp \
    Demos/fournisseurdemotactics.cpp \
    ClientGraphique/frameplayagainstcomputer.cpp \
    Algo/algocomputer.cpp \
    Algo/evaluatorposition.cpp \
    Algo/creatordatann.cpp \
    exceptionsInfos.cpp \
    Algo/ThreadAlgo.cpp \
    Preferences/applicationstyle.cpp \
    Preferences/applicationlanguage.cpp \
    ClientGraphique/mainapplicationlearnchess.cpp

HEADERS  += mainwindow.h \
    chessboardview.h \
    Data/tablepiece.h \
    Data/ArticlePiece.hpp \
    TypePiece.hpp \
    TypeInfoChessBoard.hpp \
    frameinitiation.h \
    chessboardscene.h \
    ChessBoardManager/chessmanager.h \
    ChessBoardManager/ObserverChessManager.h \
    qgraphicspieceitem.h \
    infosgraphiques.h \
    descriptordemo.h \
    framecreationechiquier.h \
    qlistepieces.h \
    ChessBoardManager/Irulemovepiece.h \
    ChessBoardManager/IChessManager.h \
    ChessBoardManager/RuleMoveRook.h \
    ChessBoardManager/FactoryRuleMovePiece.h \
    ChessBoardManager/RuleMoveKnight.h \
    ChessBoardManager/RuleMoveBishop.h \
    ChessBoardManager/RuleMoveQueen.h \
    ChessBoardManager/RuleMoveKing.h \
    ChessBoardManager/RuleMovePawn.h \
    TypeDemo.hpp \
    xmlreaderwriterfordemos.h \
    framecreationdemosmove.h \
    infosconfiguration.h \
    implinitiation.h \
    implinitiationdemomove.h \
    implinitiationtrainingmove.h \
    IChessBoardView.hpp \
    decochessboardviewwithrotation.h \
    ChessBoardManager/dialogaskpiece.h \
    ChessBoardManager/commandplay.h \
    ImportExport/pgnreaderwriter.h \
    ClientGraphique/framevisualizegames.h \
    ChessBoardManager/ILoaderGame.h \
    ClientGraphique/framemultigames.h \
    ClientGraphique/dialogaskentetefichierpgn.h \
    TypeFichierPGN.h \
    ClientGraphique/IActualisable.hpp \
    ClientGraphique/affichages.h \
    Demos/actionsdemo.h \
    ClientGraphique/painterpathmoves.h \
    ClientGraphique/dialoginfosmove.h \
    ClientGraphique/dialogexplanation.h \
    ClientGraphique/frameintroduction.h \
    Demos/IFournisseurDemo.h \
    Demos/fournisseurdemomovepiece.h \
    Demos/factoryfournisseurdemo.h \
    Demos/fournisseurdemospecialmoves.h \
    Demos/fournisseurdemopatternsmat.h \
    Demos/fournisseurdemotactics.h \
    ClientGraphique/frameplayagainstcomputer.h \
    Algo/algocomputer.h \
    Types/TypeInfosMoves.h \
    Algo/evaluatorposition.h \
    Types/TypeInfosAlgo.h \
    Algo/creatordatann.h \
    exceptionsInfos.h \
    Algo/ThreadAlgo.h \
    Algo/IObserverAlgo.h \
    IControleur.h \
    Preferences/applicationstyle.h \
    Types/TypePreferences.h \
    ClientGraphique/IUserPiecesGraphiques.h \
    Types/TypeInfosGame.h \
    Types/TypeLangage.h \
    Preferences/applicationlanguage.h \
    ClientGraphique/mainapplicationlearnchess.h \
    ClientGraphique/IUserTraductions.h

FORMS    += mainwindow.ui \
    frameinitiation.ui \
    framecreationechiquier.ui \
    framecreationdemosmove.ui \
    decochessboardviewwithrotation.ui \
    ChessBoardManager/dialogaskpiece.ui \
    ClientGraphique/framevisualizegames.ui \
    ClientGraphique/framemultigames.ui \
    ClientGraphique/dialogaskentetefichierpgn.ui \
    ClientGraphique/dialoginfosmove.ui \
    ClientGraphique/dialogexplanation.ui \
    ClientGraphique/frameintroduction.ui \
    ClientGraphique/frameplayagainstcomputer.ui

TRANSLATIONS =  translate_fr.ts \
                translate_en.ts

RESOURCES += \
    Ressources.qrc \
    RessourcesPieces.qrc

OTHER_FILES += \
    DemosMove.xml
