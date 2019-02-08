#include "automata.hpp"


bool Automata::fromSupremica(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(NULL, "Unable to open the file!",file.errorString());
        return false;
    }

    //Le fichier sera lu et son contenu sera dans le QString
    QByteArray encodedFileContent = file.readAll();
    file.close();

    //Conversion du QByteArray en QString
    QString fileContent = QTextCodec::codecForName("UTF-8")->toUnicode(encodedFileContent);

    //Nécessaire pour le parsing d'utiliser un inputSource, nous lui donnons donc le contenu de fileContent
    QXmlInputSource xml_content;
    xml_content.setData(fileContent);

    //Parsera les infos afin de vérifier la bonne concordance des infos
    QXmlSimpleReader reader;

    if(!reader.parse(xml_content))
    {
        perror("Erreur lors du parsing du fichier!\n");
        QMessageBox::information(NULL, "Unable to parse the file!",file.errorString());
        return false;
    }

    //Convertit le QByteArray en dans le QDomDocument (nous pouvons utiliser un QString au lieu du QByteArray)
    QDomDocument doc;
    doc.setContent(encodedFileContent);

    /*
    QDomNodeList list=doc.elementsByTagName("string");
    QString helloWorld=list.at(0).toElement().text();*/


    return true;
}
