#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <QMessageBox>
#include <QXmlInputSource>
#include <QTextCodec>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include "automaton.hpp"

enum file_type {NONE, SUPREMICA, SEDMA, DESUMA};

class Automata{
    private:
        QMap<int, Automaton> automatonList;
        QString name;
        int idAutomaton = 0;
        enum file_type type = NONE;
        QString filePath;

    public:
        int getIdAutomaton() {return idAutomaton;}
        void idAutomatonIncr() {idAutomaton++;}
        bool fromSupremica(QString fileName);
        void toSupremica(QXmlStreamWriter *stream);
        QMap<int, Automaton> *get_automatons();
        Automaton *get_automaton_at(int i);
        bool fromSedma(QString fileName);
        void toSedma(QString file_name);
        bool fromDesuma(QString);
        void toDesuma(QXmlStreamWriter *);
        void resetId() {idAutomaton = 0;}
        enum file_type getType() {return type;}
        void setType(enum file_type arg) {type = arg;}
        QString getFilePath() {return filePath;}
        void setFilePath(QString fileName) {filePath = fileName;}
};

#endif // AUTOMATA_HPP
