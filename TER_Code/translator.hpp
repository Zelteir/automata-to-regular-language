#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include <QString>
#include "automaton.hpp"

class Translator
{
    private:
        QString regex;
    public:
        Translator();
        QString getRegex() {return regex;}
        QString star(QString);
        void brzozowskiMethod(Automaton);
        void reverseBrzozowski(Automaton);
};

#endif // TRANSLATOR_HPP
