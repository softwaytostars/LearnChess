#include "mainapplicationlearnchess.h"
#include <QTranslator>

MainApplicationLearnChess::MainApplicationLearnChess(int &argc, char **argv)
    : QApplication (argc, argv),
      _Translator(0)
{
}

MainApplicationLearnChess::~MainApplicationLearnChess() {

    if (_Translator)
        delete _Translator;
}

void MainApplicationLearnChess::InstallTranslatorFor (const QString& aTranslationFile)
{
    if (_Translator) {
        this->removeTranslator(_Translator);
    } else
         _Translator = new QTranslator();

    _Translator->load(QString("translate_") + aTranslationFile, ":/translations");
    this->installTranslator(_Translator);
}
