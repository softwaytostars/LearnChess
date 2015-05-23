#include "descriptordemo.h"
#include "Demos/actionsdemo.h"

DescriptorDemo::DescriptorDemo():
    _UseRotation(false),
    _Explanation("")
{
}

DescriptorDemo::~DescriptorDemo()
{
    for (QVector<IActionDemo*>::const_iterator it = _ListeActionDemo.begin();
         it != _ListeActionDemo.end();
         ++it) {
        delete (*it);
    }
}
