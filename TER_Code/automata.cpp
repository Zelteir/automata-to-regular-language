#include "automata.hpp"

/*
 * Importe une liste d'automaton (automata/module) depuis un fichier XML standard généré par Supremica (version 200909171605)
*/
bool Automata::fromSupremica(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(NULL, "Unable to open the file!",file.errorString());
        return false;
    }

    //Le fichier sera lu et son contenu sera dans le QByteArray
    QByteArray encodedFileContent = file.readAll();
    file.close();

    QString error;
    QString fullError = "";
    int line, column;

    //Convertit le QByteArray en QDomDocument avec vérification de la validité du document
    QDomDocument doc;
    if(!doc.setContent(encodedFileContent, &error, &line, &column))
    {
        fullError.append("Error: ").append(error).append(" in line " ).append(QString::number(line)).append(" column ").append(QString::number(column));
        QMessageBox::information(NULL, "Unable to parse file!", fullError);
        return false;
    }

    QDomElement node = doc.firstChildElement();
    name = node.attributes().namedItem("name").nodeValue();

    //Creation of an automaton list
    QDomNodeList list=doc.elementsByTagName("Automaton");

    //Creation of an automaton object for each automaton
    int i;
    automatonList.clear();
    for(i = 0; i < list.size(); i++)
    {
        automatonList.append(Automaton(list.item(i)));
    }

    return true;
}

void Automata::toSupremica(QXmlStreamWriter *stream)
{
    stream->writeStartElement("Automata");
    stream->writeAttribute("name", this->name);
    for(Automaton a : automatonList)
        a.toSupremica(stream);
    stream->writeEndElement();
}

/*
 * Return la liste des automatons contenu dans l'automata
*/
QList<Automaton> *Automata::get_automatons()
{
    return &(this->automatonList);
}

/*
 * Return un automaton spécifique de la liste à l'indice i
*/
Automaton *Automata::get_automaton_at(int i)
{
    return &(this->automatonList[i]);
}

