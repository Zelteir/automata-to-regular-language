#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <QTextCodec>
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include "automaton.hpp"

class Automata{

    Automaton* AutomatonList;

    void fromSupremica(char* fileName);
};

#endif // AUTOMATA_HPP
