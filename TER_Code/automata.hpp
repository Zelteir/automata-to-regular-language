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
        QList<Automaton> automatonList;
        QString name;

    public:
        bool fromSupremica(QString fileName);
        void toSupremica(QXmlStreamWriter *stream);
        QList<Automaton> *get_automatons();
        Automaton *get_automaton_at(int i);
};

#endif // AUTOMATA_HPP
