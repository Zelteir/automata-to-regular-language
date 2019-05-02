#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <QMessageBox>
#include <QXmlInputSource>
#include <QTextCodec>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include "automaton.hpp"

class Automata{
    private:
        QMap<int, Automaton> automatonList;
        QString name;
        int idAutomaton = 0;

    public:
        int getIdAutomaton() {return idAutomaton;}
        void idAutomatonIncr() {idAutomaton++;}
        bool fromSupremica(QString fileName);
        void toSupremica(QXmlStreamWriter *stream);
        QMap<int, Automaton> *get_automatons();
        Automaton *get_automaton_at(int i);
        bool fromSedma(QString fileName);
        void toSedma(QXmlStreamWriter *stream);
};

#endif // AUTOMATA_HPP
