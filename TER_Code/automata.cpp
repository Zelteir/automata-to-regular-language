#include "automata.hpp"


void Automata::fromSupremica(char* fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        perror("Erreur lors de l'ouverture du fichier!\n");
    }else{
        QByteArray encodedFileContent = file.readAll();                                         //Le fichier sera lu et son contenu sera dans le QString
        file.close();

        QString fileContent = QTextCodec::codecForName("UTF-8")->toUnicode(encodedFileContent); //Conversion du QByteArray en QString

        QXmlInputSource xml_content;                                                            //Nécessaire pour le parsing d'utiliser un inputSource, nous lui donnons donc le contenu de fileContent
        xml_content.setData(fileContent);

        QXmlSimpleReader reader;                                                                //Parsera les infos

        if(!reader.parse(xml_content))
        {
            perror("Erreur lors du parsing du fichier!\n");
        }else{

        }
    }
}
