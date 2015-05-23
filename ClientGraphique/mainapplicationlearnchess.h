#ifndef MAINAPPLICATIONLEARNCHESS_H
#define MAINAPPLICATIONLEARNCHESS_H

#include <QtGui/QApplication>

class MainApplicationLearnChess : public QApplication
{
public:
    MainApplicationLearnChess(int &argc, char **argv);
    ~MainApplicationLearnChess();
    void InstallTranslatorFor (const QString& aTranslationFile);
private:
    QTranslator* _Translator;

};

#endif // MAINAPPLICATIONLEARNCHESS_H
