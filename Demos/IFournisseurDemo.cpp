#include "IFournisseurDemo.h"

IFournisseurDemo::IFournisseurDemo():
    _SelectedRow(-1)
{
}

IFournisseurDemo::~IFournisseurDemo()
{
}

void IFournisseurDemo::AssociateIndexDemoAndXMLNodeName (int aIndex, const QString& aXMLNodeName) {

    _mapIndexDemotoXMLNameNodeName.insert(bimap_IndexDemotoXMLNameNodeName::value_type (aIndex,   aXMLNodeName ));
}

bool IFournisseurDemo::get_IndexDemoForXMLNodeName (const QString& aXMLNodeName, int& aIndexDemo) const {

    bimap_IndexDemotoXMLNameNodeName::right_map::const_iterator it = _mapIndexDemotoXMLNameNodeName.right.find(aXMLNodeName);

    if (it != _mapIndexDemotoXMLNameNodeName.right.end()) {
        aIndexDemo = (*it).second;
        return true;
    }else
        return false;
}
bool IFournisseurDemo::get_XMLNodeNameForIndexDemo (int aIndexDemo, QString& aXMLNodeName) const {


    bimap_IndexDemotoXMLNameNodeName::left_map::const_iterator it = _mapIndexDemotoXMLNameNodeName.left.find(aIndexDemo);

    if (it != _mapIndexDemotoXMLNameNodeName.left.end()) {
        aXMLNodeName = (*it).second;
        return true;
    }else
        return false;
}

void IFournisseurDemo::AssociateIndexDemoAndTypeDemoName (int aIndex, const QString& aTypeDemoName) {

    _mapIndexDemotoTypeDemoName.insert(bimap_IndexDemotoTypeDemoName::value_type (aIndex,   aTypeDemoName ));
    _ListeNames.push_back(aTypeDemoName);
}


bool IFournisseurDemo::get_IndexDemoForTypeDemoName (const QString& aTypeDemoName, int& aIndexDemo) const {

    bimap_IndexDemotoTypeDemoName::right_map::const_iterator it = _mapIndexDemotoTypeDemoName.right.find(aTypeDemoName);

    if (it != _mapIndexDemotoTypeDemoName.right.end()) {
        aIndexDemo = (*it).second;
        return true;
    }else
        return false;
}
bool IFournisseurDemo::get_TypeDemoNameForIndexDemo (int aIndexDemo, QString& aTypeDemoName) const {


    bimap_IndexDemotoTypeDemoName::left_map::const_iterator it = _mapIndexDemotoTypeDemoName.left.find(aIndexDemo);

    if (it != _mapIndexDemotoTypeDemoName.left.end()) {
        aTypeDemoName = (*it).second;
        return true;
    }else
        return false;
}

void IFournisseurDemo::translate() {

    _mapIndexDemotoTypeDemoName.clear();
    _ListeNames.clear();
    AjouterTypeDemoNames ();
}
