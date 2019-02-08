#include "automata.hpp"

void fromXML(QString fileName)
{
    QXmlInputSource xml_content;    //Nécessaire pour le parsing d'utiliser un inputSource
    xml_content.setData(fileName);

    QXmlSimpleReader reader;        //Parsera les infos
    reader.parse(xml_content);


}
