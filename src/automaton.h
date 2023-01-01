/**
* \file automaton.h
* \brief Reprezentace celularniho automatu.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef AUTOMATON_H_INCLUDED
#define AUTOMATON_H_INCLUDED
#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <map>
#include <QMetaType>
#include "config.h"

/// Konstanty pro oznaceni pozice bunek.
enum E_CELL_DIRECTION
{
    DIR_TOP_LEFT = 0,     ///< Horni levy soused.
    DIR_TOP = 1,          ///< Horni soused.
    DIR_TOP_RIGHT = 2,    ///< Horni pravy soused.
    DIR_LEFT = 3,         ///< Levy soused.
    DIR_CENTRAL = 4,      ///< Centralni bunka.
    DIR_RIGHT = 5,        ///< Pravy soused.
    DIR_BOTTOM_LEFT = 6,  ///< Dolni levy soused.
    DIR_BOTTOM = 7,       ///< Dolni soused.
    DIR_BOTTOM_RIGHT = 8, ///< Dolni pravy soused.
    DIR_COUNT = 9         ///< Pocet variant smeru.
};

/// Konstanty pro oznaceni uvazovaneho sousedstvi bunek.
enum E_NEIGHBOURHOOD
{
    NEIGHBOURHOOD_UNKNOWN = -1,     ///< Nenastavene okoli.
    NEIGHBOURHOOD_VON_NEUMANN = 0,  ///< Von-Neumannovo okoli.
    NEIGHBOURHOOD_MOORE = 1,        ///< Moorovo okoli.
    NEIGHBOURHOOD_COUNT = 2         ///< Pocet variant okoli.
};

/// Celularni automat (abstraktni trida).
class Automaton
{
    public:

    /// Konstanta informujici, ze nedoslo k zadne zmene
    const static unsigned char STATE_UNCHANGED = 255;

    /// Typ pro ulozeni hodnoty bunky automatu.
    typedef unsigned char CellType;

    /// Typ pro ulozeni radku hodnot bunek automatu.
    typedef std::vector<CellType> CellVector;

    /// Typ pro ulozeni mrizky automatu.
    typedef std::vector<CellVector> GridType;

    /// Typ pro ulozeni fitness kazde bunky automatu.
    typedef std::vector<int> CellFitness;

    /// Typ pro ulozeni informaci o hammingove vzdalenosti dvou automatu v zavislosti na kroku automatu.
    typedef std::vector<double> HammingDistanceStatistic;

    /// Typ pro ulozeni instrukce.
    typedef struct
    {
        CellType config[DIR_COUNT];    ///< Konfigurace.
        CellType output;               ///< Vystupni hodnota pro danou vstupni kombinaci.
        E_NEIGHBOURHOOD neighbourhood; ///< Typ okoli.
        int id;                        ///< ID instrukce.
        std::string name;              ///< Nazev instrukce.
        bool active;                   ///< Priznak stavu instrukce (smazana / nesmazana)
    } Instruction;

    /// Typ pro ulozeni bodu v 2D prostoru.
    typedef struct
    {
        int x;
        int y;
    } Point;

    /// Typ pro ulozeni chromozomu (vybrane posloupnosti instrukci k rizeni automatu).
    typedef struct
    {
        std::vector<Instruction *> instrList;
        float fitness;
    } Chromosome;

    typedef struct std::vector<Chromosome> ChromosomeSet;

    /// Typ pro ulozeni posloupnosti instrukci k rizeni automatu.
    typedef std::vector<std::vector<Instruction> > InstructionSet;

    /// Typ pro ulozeni vektoru chromozomu kodujicich instrukce k rizeni automatu.
    typedef std::vector<InstructionSet> InstructionSetVector;

    Config *config;               ///< Odkaz na instanci tridy s konfiguraci pro zjednoduseni pristupu.
    GridType grid;                ///< Mrizka automatu.
    GridType grid2;               ///< Mrizka automatu v 1 kroku v minulosti.
    GridType grid3;               ///< Mrizka automatu v 2 krocich v minulosti.
    GridType grid4;               ///< Mrizka automatu v 3 krocich v minulosti.
    GridType mapGrid;             ///< Mrizka pro stavy urcujici, zda-li byla na dane pozici aplikovana instrukce.

    /**
    * Vytiskne mrizku automatu na standardni vystup.
    */
    void PrintGrid();

    /**
     * Nastavi mrizku automatu.
     * @param grid Mrizka, ktera bude nastavena do automatu.
     */
    void SetGrid(Automaton::GridType grid);

    /**
     * Provede urceny pocet prechodu celularniho automatu.
     * @param stepCount Pozadovany pocet prechodu.
     */
    virtual void AutomatonStep(int stepCount) = 0;

    /**
     * Provede urceny pocet prechodu celularniho automatu.
     * @param stepCount Pozadovany pocet prechodu.
     * @param cellPossibilities Vektor s mnozinou moznych stavu bunek automatu.
     * @param damageProbability Pravdepodobnost zmeny hodnoty bunky.
     */
    virtual void AutomatonStep(int stepCount, std::vector<int> &cellPossibilities, double damageProbability) = 0;

    /**
     * Zjisti shodu dvou automatu (pro variantu automatu vyuzivajici klasicky algoritmus genetickeho programovani).
     * @param automaton1 Prvni porovnavany automat.
     * @param automaton2 Druhy porovnavany automat.
     * @return Shoda porovnavanych automatu (cim vetsi cislo, tim lepe).
     */
    static int HammingDistanceGP(GridType grid1, GridType grid2);

    /**
     * Zjisti shodu dvou automatu (pro variantu automatu vyuzivajici algoritmus celularniho programovani).
     * @param automaton1 Prvni porovnavany automat.
     * @param automaton2 Druhy porovnavany automat.
     * @param chromosomeSet Posloupnost instrukci pro jednotlive bunky automatu (k nim se ulozi fitness).
     * @return Shoda porovnavanych automatu (cim vetsi cislo, tim lepe).
     */
    static int HammingDistanceCP(GridType grid1, GridType grid2, Automaton::ChromosomeSet &chromosomeSet);

    /**
     * Zjisti shodu dvou automatu na vyznacenych pozicich
     * @param automaton1 Prvni porovnavany automat.
     * @param automaton2 Druhy porovnavany automat.
     * @param positionsAdder Pozice, na kterych dochazi k porovnani.
     * @return Shoda porovnavanych automatu (cim vetsi cislo, tim lepe).
     */
    static int HammingDistanceComputationTask(GridType grid1, GridType grid2, std::vector<Point> positionsAdder);

    protected:
    int cellCount;                 ///< Pocet bunek automatu.
    E_NEIGHBOURHOOD neighbourhood; ///< Pocet prvku ovlivnujici nasledujici stav bunky (pocet sousedu).
    GridType pattern;              ///< Hledany vzor.

    /**
     * Provede prechod u jedne bunky - zmeni jeji stav podle jejiho okoli.
     * @param cellNumber Cislo bunky.
     * @param newGrid Pomocna mrizka, do ktere jsou zapisovany nove stavy bunek.
     * @param actConfiguration Aplikovana konfigurace.
     */
    virtual void SetNewConfiguration(GridType &mapGrid, int cellNumber, GridType &newGrid, Instruction *actInstruction) = 0;

    /**
     * Ziska novy stav bunky na zaklade aktualniho stavu a instrukci z chromozomu.
     * @param actConfig Pole s hodnotami sousednich bunek.
     * @param actInstruction Aplikovana konfigurace.
     * @return Novy stav bunky.
     */
    virtual CellType GetNextState(CellType actConfig[], Instruction *actInstruction) = 0;
};

Q_DECLARE_METATYPE(Automaton::GridType)

Q_DECLARE_METATYPE(Automaton::CellFitness)

#endif //AUTOMATON_H_INCLUDED

