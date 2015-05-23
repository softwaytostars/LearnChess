#ifndef TYPELANGAGE_H
#define TYPELANGAGE_H

#include <QMap>

enum eLangue {
    eLangueFR,
    eLangueEN
};

typedef QMap<eLangue, QString> tMapPrefLangueName;

#endif // TYPELANGAGE_H
