#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include "automaton.hpp"

class Automata{

    Automaton* AutomatonList;

    void fromXML(QString fileName);
};

#endif // AUTOMATA_HPP
