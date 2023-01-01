/**
* \file evolutionManagerGP.h
* \brief Spravce evoluce instrukci (s vyuzitim klasickeho algoritmu genetickeho programovani).
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef EVOLUTION_MANAGER_GP_H_INCLUDED
#define EVOLUTION_MANAGER_GP_H_INCLUDED
#include <iostream>
#include <iomanip>
#include <math.h>
#include <time.h>
#include <QtGui>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMetaType>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <sstream>
#include "automaton.h"
#include "automatonGP.h"
#include "config.h"
#include "xmlConfig.h"
#include "evolutionManager.h"
#include "recombinationOperator.h"

/// Spravce evoluce instrukci (s vyuzitim klasickeho algoritmu genetickeho programovani).
class EvolutionManagerGP : public QThread, EvolutionManager
{
    Q_OBJECT

    /// Vysledky evolucniho behu (pro variantu pouzivajici klasicky geneticky algoritmus).
    typedef struct
    {
        int crossoverCount;                ///< Pocet aplikaci operatoru krizeni od inicializace.
        int mutationCount;                 ///< Pocet aplikaci operatoru mutace od inicializace.
        int evaluationCount;               ///< Pocet ohodnoceni chromozomu od inicializace.
        float avgPopulationFitness;        ///< Prumerne fitness populace od inicializace.
        float minPopulationFitness;        ///< Maximalni fitness populace od inicializace..
        float maxPopulationFitness;        ///< Minimalni fitness populace od inicializace.
        float avgActPopulationFitness;     ///< Prumerne fitness v aktualni populaci.
        float minActPopulationFitness;     ///< Nejlepsi fitness v aktualni populaci.
        float maxActPopulationFitness;     ///< Nejhorsi fitness v aktualni populaci.
        float avgInitialPopulationFitness; ///< Prumerne fitness v pocatecni populaci.
        float minInitialPopulationFitness; ///< Nejlepsi fitness v pocatecni populaci.
        float maxInitialPopulationFitness; ///< Nejhorsi fitness v pocatecni populaci.
        struct timeval runningTime;        ///< Cas behu vypoctu.
    } tResult;

    Automaton::Chromosome *resultChromosome;      ///< Vysledny chromozom s instrukcemi k reseni ulohy.
    std::vector<Automaton::Chromosome> parentPop; ///< Populace rodicu - mnoziny chromozomu s instrukcemi.
    std::vector<Automaton::Chromosome> childPop;  ///< Populace potomku - mnoziny chromozomu s instrukcemi.
    std::vector<Automaton::Chromosome> tempPop;   ///< Populace rodicu a potomku vybranych do dalsiho cyklu evolucniho behu.
    tResult result;                               ///< Vysledky vypoctu.
    struct timeval startTime;                     ///< Cas zacatku spusteni vypoctu.
    std::vector<float> tempAvgPopulationFitness;
    std::vector<float> tempMinPopulationFitness;
    std::vector<float> tempMaxPopulationFitness;

    QMutex mutex;
    QWaitCondition condition;

    bool abortFlag;                      ///< Priznak ukonceni evolucnich vypoctu.
    bool sleepFlag;                      ///< Priznak pozastaveni evolucnich vypoctu.

    /**
     * Vytvori populaci jedincu (mnozinu chromozomu s instrukcemi).
     */
    virtual void GenerateInitPopulation();

    /**
     * Ohodnoti populaci jedincu.
     * @param population Populace jedincu - sekvence chromozomu s instrukcemi.
     */
    virtual void EvaluatePopulation(std::vector<Automaton::Chromosome> &population);

    /**
     * Ziska cislo aktualni generace behu vypoctu.
     * @return Urcuje, zda-li jiz bylo dosazeno pozadovaneho poctu generaci.
     */
    bool Done();

    /**
     * Provede jeden evolucni krok.
     */
    void Step();

    /**
     * Ohodnoti jedince v populaci.
     * @param chromosome Ohodnocovany jedinec v populaci.
     */
    void EvaluateChromosome(Automaton::Chromosome &chromosome);

    /**
     * Provede krizeni dvou chromozomu s instrukcemi za pomoci nahodne zvolene metody krizeni.
     * @param parent1 Prvni z krizenych chromozomu.
     * @param parent2 Druhy z krizenych chromozomu.
     * @param child1 Prvni chromozom vznikly po aplikaci operatoru krizeni.
     * @param child2 Druhy chromozom vznikly po aplikaci operatoru krizeni.
     */
    void Crossover(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2);

    /**
     * Provede s urcitou pravdepodobnosti mutaci chromozomu s instrukcemi.
     * @param chromosome Mutovany chromozom s instrukcemi.
     */
    void Mutation(Automaton::Chromosome &chromosome);

    /**
     * Provede turnajovou selekci nad danou populaci a urci viteze.
     * @param population Populace, nad kterou selekce probiha.
     * @param tournamentSize Pocet ucastniku turnaje.
     * @return Vitez turnaje (chromosom s nejvyssim fitness).
     */
    int TournamentSelection(std::vector<Automaton::Chromosome> &population);

    /**
     * Zajisti obnovu populace.
     * @param population Populace, do ktere se pridavaji selektovani potomci.
     * @param selectedPopulation Populace, ze ktere se vybiraji selektovani potomci.
     */
    void Survive(std::vector<Automaton::Chromosome> &population, std::vector<Automaton::Chromosome> &selectedPopulation);

    /**
     * Ulozi vysledky k dane generaci behu vypoctu.
     */
    void SetGenerationStatistics();

    public:
    EvolutionManagerGP(QObject *parent = 0);
    ~EvolutionManagerGP();

    /**
     * Hleda vhodne chromozomy s instrukcemi k rizeni CA.
     * @param instrList Seznam instrukci potencialne vyuzitelnych k sestaveni chromozomu.
     */
    virtual void FindSolution(std::vector<Automaton::Instruction> instrList);

    /**
     * Ulozi nejuspenejsi chromozom s mnozinou instrukci do souboru.
     * @param Cesta k vystupnimu souboru.
     * @return Udava, zda se podarilo ulozit nejuspesnejsi chromozom s mnozinou instrukci do souboru.
     */
    virtual bool SaveChromosome(std::string outputFile);

    /**
     * Pozastavi evoluci instrukci.
     */
    virtual void StopEvoluteInstructions();

    /**
     * Zrusi evoluci instrukci.
     */
    virtual void AbortEvoluteInstructions();

    /**
     * Smycka evolucniho vypoctu.
     */
    void run();

    signals:
    /**
     * Signal zaslany po provedeni urceneho poctu evolucnich kroku nebo splneni podminky ukonceni evoluce.
     * @param avgCurrentPopulationFitness Prumerne reseni v aktualnim evolucnim kroku.
     * @param maxCurrentPopulationFitness Nejlepsi reseni v aktualnim evolucnim kroku.
     */
    void CurrentChromosomeGenerated(float avgActPopulationFitness, float maxActPopulationFitness);

    /**
     * Signal zaslany po dokonceni behu evoluce.
     * @param basicInstructionSet Indikuje, zda-li se byla generovana zakladni sada instrukci nebo sada instrukci pro sebeopravu (pouze v pripade vysetrovani sebe-organizace).
     */
    void ChromosomeGenerated(bool basicInstructionSet);
};

/**
 * Porovna dva ruzne chromozomy na zaklade jejich prirazene fitness hodnoty.
 * @param inv1 Prvni porovnavany jedinec.
 * @param inv2 Druhy porovnavany jedinec.
 * @return True, pokud je fitness prvniho jedince mensi nez fitness druheho jedince.
 */
bool FitnessSortGP(Automaton::Chromosome inv1, Automaton::Chromosome inv2);

#endif //EVOLUTION_MANAGER_CP_H_INCLUDED
