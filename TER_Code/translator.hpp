#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include <QString>
#include <memory>
#include "automaton.hpp"

class Translator
{
    private:
        QString regex;
    public:
        Translator();
        QString getRegex() {return regex;}
        QString star(QString);
        QVector<std::shared_ptr<QString>> starV2(QVector<std::shared_ptr<QString>>);
        void brzozowskiMethod(Automaton, bool, bool);
        void reverseBrzozowski(Automaton, bool, bool);
        void brzozowskiMethodV2(Automaton, bool, bool);
        void reduction(Automaton);
        void transitive_Closure(Automaton, bool, bool);
        Automaton automatonMinimization(Automaton);
        QList<QList<int>> split(QList<int>, QList<int>, Event e, QVector<QVector<QString>>);
        Automaton reconstruct(QList<QList<int>>, Automaton, QVector<QVector<QString>>);
        Automaton automatonMinimizationV2(Automaton param);
        bool equivalent(int, int, QList<QList<int>>, QVector<QVector<QString>>);
};

#endif // TRANSLATOR_HPP
