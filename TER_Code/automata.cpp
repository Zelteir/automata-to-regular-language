#include "automata.hpp"

#include <QFileDialog>
#include <QFileInfo>

/*
 * Importe une liste d'automaton (automata/module) depuis un fichier XML standard généré par Supremica (version 200909171605)
*/
bool Automata::fromSupremica(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(nullptr, "Unable to open the file!",file.errorString());
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
        QMessageBox::information(nullptr, "Unable to parse file!", fullError);
        return false;
    }

    this->type = SUPREMICA;
    this->filePath = fileName;

    resetId();

    QDomElement node = doc.firstChildElement();
    name = node.attributes().namedItem("name").nodeValue();

    //Creation of an automaton list
    QDomNodeList list=doc.elementsByTagName("Automaton");

    //Creation of an automaton object for each automaton
    int i;
    automatonList.clear();
    QVector<Automaton> aList(list.size());
    for(i = 0; i < list.size(); i++)
    {
        aList[i].fromSupremica(idAutomaton, list.item(i));
        automatonList.insert(idAutomaton, aList[i]);
        idAutomatonIncr();
    }

    return true;
}

/*
 * Export current automata to a Supremica-compatible XML file
 */
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
QMap<int, Automaton> *Automata::get_automatons()
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


/*
 * Import an automaton from a '.automata' file created by SEDMA
 * Since those files contains only one automaton, no loop is necessary to scan the file
*/
bool Automata::fromSedma(QString fileName)
{
    QFile file(fileName);
    QString line;
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(nullptr, "Unable to open the file!",file.errorString());
        return false;
    }
    this->type = SEDMA;
    this->filePath = fileName;

    Automaton a;
    a.fromSedma(idAutomaton, fileName, QString(file.readAll()));
    automatonList.clear();
    automatonList.insert(idAutomaton, a);
    idAutomatonIncr();

    return true;
}

/*
 * Export the current automaton to a SEDMA-compatible '.automata' file
 * Since '.automata' files can only contain one automaton, if there is more in the current automata
 * we need to create a new file for each of them
*/
void Automata::toSedma(QString file_name)
{
    QTextStream stream;
    QFile file;
    QString dir;
    QFileInfo fInfo;
    QString tmpPath;
    if(!file_name.isEmpty())    //if the automata is from a SEDMA file or there is only one automaton we save the first automaton in it
    {
        file.setFileName(file_name);
        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::information(nullptr, "Unable to open the file!",file.errorString());
            return;
        }
        stream.setDevice(&file);
        automatonList[0].toSedma(&stream);
        file.close();
    }
    if(automatonList.size() > 1)    //if there is more than one automaton, need to create separate files
    {

        if(!file_name.isEmpty())    //if the automata if from a SEDMA file, take it's path to save the new files in the same folder
        {
            fInfo.setFile(file_name);
            dir = "" + fInfo.path() + "\\";
        }
        else    //otherwise ask for a folder
        {
            dir = "" + QFileDialog::getExistingDirectory(nullptr, "Select Directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "\\";
        }
        for(Automaton a : automatonList)
        {
            stream.flush();
            if(!file_name.isEmpty() && a.getId() == 0)  //this automaton is already saved
            {
                continue;
            }
            tmpPath = "" + dir + a.getName() + ".automata";
            file.setFileName(tmpPath);
            if(!file.open(QIODevice::WriteOnly)){
                QMessageBox::information(nullptr, "Unable to open the file!",file.errorString());
                return;
            }
            stream.setDevice(&file);
            a.toSedma(&stream);
            file.close();
        }
    }
}

/*TO DO*/
bool Automata::fromDesuma(QString)
{

}

/*TO DO*/
void Automata::toDesuma(QXmlStreamWriter *)
{

}
