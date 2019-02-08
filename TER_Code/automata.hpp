#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <QMessageBox>
#include <QXmlInputSource>
#include <QTextCodec>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include "automaton.hpp"

class Automata{
    private:
        Automaton* AutomatonList;

    public:
        bool fromSupremica(QString fileName);
};

#endif // AUTOMATA_HPP
