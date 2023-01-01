/**
* \file evolutionManager.h
* \brief Sprava evoluce instrukci.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef EVOLUTION_MANAGER_H_INCLUDED
#define EVOLUTION_MANAGER_H_INCLUDED
#include <QtGui>
#include <iostream>
#include <sstream>
#include "automaton.h"
#include "config.h"
#include "limits.h"

/// Spravce evoluce instrukci.
class EvolutionManager
{
    protected:
    int cellCount;                   ///< Pocet bunek automatu
    Automaton::GridType whiteGrid;   ///< Pomocna mrizka zaplnena bilymi polickami.
    Automaton::GridType blackGrid;   ///< Pomocne mrizka zaplnena cernymi polickami.
    Automaton::GridType zygote;      ///< Zygota (uloha sebe-organizace).
    Automaton::GridType pattern;     ///< Hledany vzor (uloha sebe-organizace).
    std::vector<Automaton::GridType> trainSet;       ///< Trenovaci mnozina konfiguraci.
    std::vector<Automaton::GridType> patternSet;     ///< Trenovaci mnozina konfiguraci s pozadovanym vysledkem.
    std::vector<Automaton::CellType> trainSetResult; ///< Pozadovane vysledky u jednotlivych problemu majority z trenovaci mnoziny
    std::vector<Automaton::Point> positionsAdder;  ///< Pozice v mrizce, ktere maji byt porovnavany pri zjistovani fitness (pro ulohu mechanickych vypoctu).
    Config *config;                                ///< Odkaz na instanci tridy s konfiguraci pro zjednoduseni pristupu.
    int actGeneration;                             ///< Cislo aktualni generace behu evoluce.
    struct timeval startTime;                      ///< Cas zacatku spusteni vypoctu.
    std::vector<Automaton::Instruction> instrList; ///< Seznam instrukci potencialne vyuzitelnych k sestaveni chromozomu.
    Automaton::Instruction nopInstruction;         ///< Instrukce no-operation.
    std::vector<int> cellPossibilities;            ///< Vektor s moznymi stavy bunek mrizky automatu.

    EvolutionManager();

    /**
     * Vygenerovani trenovaci mnoziny pocatecnich konfiguraci automatu podle zvoleneho typu ulohy.
     */
    void GenerateTrainSet();

    /**
     * Vygenerovani mnoziny s poskozenymi cilovymi konfiguracemi (pro trenovani schopnosti sebe-opravy).
     * @param damagedSet Mnozina s poskozenymi cilovymi konfiguracemi.
     */
    void GenerateDamagedSet(std::vector<Automaton::GridType> &damagedSet);

    /**
     * Nacte ulohu ze souboru.
     * @param inputFile Cesta k vstupnimu souboru.
     * @param cellPossibilities Mnozina moznych stavu mrizky automatu.
     * @return Udava, zda se podarilo nacist ulohu ze souboru.
     */
    bool LoadSelfOrganizationTask(std::string inputFile, std::vector<int> &cellPossibilities);

    /**
     * Ziska textovou podobu zadaneho chromozomu.
     * @param chromosome Zadany chromozom.
     * @return Vypis zadane instrukci zadaneho chromozomu.
     */
    std::string PrintChromosome(Automaton::Chromosome &chromosome);

    /**
     * Vytvori populaci jedincu (mnozinu chromozomu s instrukcemi).
     */
    virtual void GenerateInitPopulation() = 0;

    /**
     * TODO
     */
    Automaton::GridType GenerateTrainGridAdder(Automaton::CellType var_A, Automaton::CellType var_B, Automaton::CellType var_Ci, Automaton::CellType var_C0, Automaton::CellType var_S);

    public:
    /**
     * Hleda vhodne chromozomy s instrukcemi k rizeni CA.
     * @param instrList Seznam instrukci potencialne vyuzitelnych k sestaveni chromozomu.
     */
    virtual void FindSolution(std::vector<Automaton::Instruction> instrList) = 0;

    /**
     * Ulozi nejuspenejsi chromozom s posloupnostmi instrukci jednotlivych bunek do souboru.
     * @param Cesta k vystupnimu souboru.
     * @return Udava, zda se podarilo ulozit nejuspesnejsi chromozom s posloupnostmi instrukci jednotlivych bunek do souboru.
     */
    virtual bool SaveChromosome(std::string outputFile) = 0;

    /**
     * Pozastavi evoluci instrukci.
     */
    virtual void StopEvoluteInstructions() = 0;

    /**
     * Zrusi evoluci instrukci.
     */
    virtual void AbortEvoluteInstructions() = 0;
};

#endif //EVOLUTION_MANAGER_H_INCLUDED
