/**
* \file automatonGP.h
* \brief Reprezentace celularniho automatu (pro vyuziti klasickym algoritmem genetickeho programovani).
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef AUTOMATON_GP_H_INCLUDED
#define AUTOMATON_GP_H_INCLUDED
#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <map>
#include "automaton.h"
#include "config.h"

/// Celularni automat (pro vyuziti klasickym algoritmem genetickeho programovani).
class AutomatonGP : public Automaton
{
    public:
    /**
     * Konstruktor.
     * @param zygote Zygota.
     * @param chromosome Chromosom s instrukcemi pro rizeni automatu.
     * @param neighbourhood Pocet prvku ovlivnujici nasledujici stav bunky (pocet sousedu).
     */
    AutomatonGP(GridType &zygote, Chromosome &chromosome, E_NEIGHBOURHOOD neighbourhood);

    /**
     * Provede urceny pocet prechodu celularniho automatu.
     * @param stepCount Pozadovany pocet prechodu.
     */
    virtual void AutomatonStep(int stepCount);

    /**
     * Provede urceny pocet prechodu celularniho automatu.
     * @param stepCount Pozadovany pocet prechodu.
     * @param cellPossibilities Vektor s mnozinou moznych stavu bunek automatu.
     * @param damageProbability Pravdepodobnost zmeny hodnoty bunky.
     */
    virtual void AutomatonStep(int stepCount, std::vector<int> &cellPossibilities, double damageProbability);

    private:
    Chromosome chromosome;         ///< Chromozom s instrukcemi pro rizeni automatu.

    /**
     * Provede prechod u jedne bunky - zmeni jeji stav podle jejiho okoli.
     * @param cellNumber Cislo bunky.
     * @param newGrid Pomocna mrizka, do ktere jsou zapisovany nove stavy bunek.
     * @param actConfiguration Aplikovana konfigurace.
     */
    virtual void SetNewConfiguration(GridType &mapGrid, int cellNumber, GridType &newGrid, Automaton::Instruction *actInstruction);

    /**
     * Ziska novy stav bunky na zaklade aktualniho stavu a instrukci z chromozomu.
     * @param actConfig Pole s hodnotami sousednich bunek.
     * @param actInstruction Aplikovana konfigurace.
     * @return Novy stav bunky.
     */
    virtual Automaton::CellType GetNextState(CellType actConfig[], Instruction *actInstruction);
};

#endif //AUTOMATON_GP_H_INCLUDED

