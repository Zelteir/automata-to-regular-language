#include <QXmlSimpleReader>
#include "automaton.hpp"
#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

class Automata{

    Automaton* listeAutomatons;
    void fromXML();
};

#endif // AUTOMATA_HPP
