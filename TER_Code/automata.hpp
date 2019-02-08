#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <QXmlInputSource>
#include <QTextCodec>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include "automaton.hpp"

class Automata{

    private:
        Automaton* AutomatonList;

    public:
        void fromSupremica(char* fileName);
};

#endif // AUTOMATA_HPP
