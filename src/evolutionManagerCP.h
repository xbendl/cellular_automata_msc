/**
* \file evolutionManagerCP.h
* \brief Spravce evoluce pravidel (s vyuzitim algoritmu celularniho programovani).
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef EVOLUTION_MANAGER_CP_H_INCLUDED
#define EVOLUTION_MANAGER_CP_H_INCLUDED
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
#include "automatonCP.h"
#include "config.h"
#include "xmlConfig.h"
#include "evolutionManager.h"
#include "recombinationOperator.h"

/// Spravce evoluce instrukci (s vyuzitim algoritmu celularniho programovani).
class EvolutionManagerCP : public QThread, EvolutionManager
{
    Q_OBJECT

    int initialStep;                         ///< Krok, od ktereho se meri uspesnost pravidel / mrizky a je uvazovano o jejich restartu
    double bestChromosomeSetFitness;         ///< Nejlepsi fitness posloupnosti instrukci automatu dosazene evoluci.
    Automaton::ChromosomeSet chromosomeSet;     ///< Sekvence chromozomu urcenych pro jednotlive bunky automatu.
    Automaton::ChromosomeSet bestChromosomeSet; ///< Nejuspesnejsi nalezena sekvence chromozomu urcenych pro jednotlive bunky automatu.
    double bestChromosomeSetFitnessRepairment;            ///< Nejlepsi fitness posloupnosti instrukci automatu dosazene evoluci (pro ulohu sebe-organizace pri treninku sebe-opravy).
    Automaton::ChromosomeSet chromosomeSetRepairment;     ///< Sekvence chromozomu urcenych pro jednotlive bunky automatu (pro ulohu sebe-organizace pri treninku sebe-opravy).
    Automaton::ChromosomeSet bestChromosomeSetRepairment; ///< Nejuspesnejsi nalezena sekvence chromozomu urcenych pro jednotlive bunky automatu (pro ulohu sebe-organizace pri treninku sebe-opravy).
    bool selfRepairmentLoop;

    /// Vysledky evolucniho behu (pro variantu pouzivajici algoritmus celularniho programovani).
    typedef struct
    {
        int crossoverCount;          ///< Pocet aplikaci operatoru krizeni od inicializace.
        int mutationCount;           ///< Pocet aplikaci operatoru mutace od inicializace.
        int evaluationCount;         ///< Pocet ohodnoceni chromozomu od inicializace.
        float avgFitness;            ///< Prumerne fitness populace od inicializace.
        struct timeval runningTime;  ///< Cas behu vypoctu.
    } tResult;

    tResult result;                               ///< Vysledky vypoctu.
    struct timeval startTime;                     ///< Cas zacatku spusteni vypoctu.
    std::vector<int> prevFitness;                 ///< Pomocna pro uchovani fitness jednotlivych bunek v predchozim evolucnim kroku.
    std::vector<int> maxFitness;                  ///< Pocet po sobe jdoucich stavu, v kterych ma bunka cilovou hodnotu
    std::vector<int> fitnessSum;                  ///< Pomocna promenna pro soucty fitness.
    std::vector<double> tempAvgFitness;
    std::vector<double> tempAvgFitnessRepairment;

    QMutex mutex;
    QWaitCondition condition;

    bool abortFlag;                      ///< Priznak ukonceni evolucnich vypoctu.
    bool sleepFlag;                      ///< Priznak pozastaveni evolucnich vypoctu.

    /**
     * Vytvori populaci jedincu (mnozinu chromozomu s instrukcemi).
     */
    virtual void GenerateInitPopulation();

    /**
     * Vytvori populaci jedincu pro trenovani sebe-opravy v pripade ulohy sebe-organize (mnozinu chromozomu s instrukcemi).
     */
    void GenerateDamagedPopulation();

    /**
     * Ziska cislo aktualni generace behu vypoctu.
     * @return Urcuje, zda-li jiz bylo dosazeno pozadovaneho poctu generaci.
     */
    bool Done(std::vector<double> &tempAvgFitness);

    /**
     * Provede jeden evolucni krok.
     * Provede jeden evolucni krok pro ulohu majority / synchronizace / sebe-organizace / sebe-opravy.
     * @param bestChromosomeSet Nejlepsi nalezena sekvence chromozomu urcenych pro jednotlive bunky automatu.
     * @param bestChromosomeSetFitness Nejlepsi fitness posloupnosti instrukci automatu dosazene evoluci.
     * @param tempAvgFitness Uloziste pro prumerne fitness v aktualnim kroku evolucniho cyklu.
     * @return Udava, zda-li jiz bylo dosazeno maximalni fitness -> pak muze byt evolucni cyklus prerusen.
     */
    void Step(Automaton::ChromosomeSet &chromosomeSet, Automaton::ChromosomeSet &bestChromosomeSet, double &bestChromosomeSetFitness, std::vector<double> &tempAvgFitness);

    /**
     * Provede jeden evolucni krok pro ulohu mechanickeho vypoctu.
     * @param chromosomeSet Sekvence chromozomu urcenych pro jednotlive bunky automatu.
     * @param bestChromosomeSet Nejlepsi nalezena sekvence chromozomu urcenych pro jednotlive bunky automatu.
     * @param bestChromosomeSetFitness Nejlepsi fitness posloupnosti instrukci automatu dosazene evoluci.
     * @param tempAvgFitness Uloziste pro prumerne fitness v aktualnim kroku evolucniho cyklu.
     * @return Udava, zda-li jiz bylo dosazeno maximalni fitness -> pak muze byt evolucni cyklus prerusen.
     */
    void StepComputation(Automaton::ChromosomeSet &chromosomeSet, Automaton::ChromosomeSet &bestChromosomeSet, double &bestChromosomeSetFitness, std::vector<double> &tempAvgFitness);

    /**
     * Ohodnoti vyvinute reseni.
     * @param chromosomeSet Sekvence chromozomu pro jednotlive bunky automatu.
     * @return Prumerne fitness pro danou mnozinu chromozomu.
     */
    double EvaluateSolution(Automaton::ChromosomeSet &chromosomeSet);

    /**
     * Nastavuje pro kazdou bunku po kazdem vyvojovem kroku pocet po sobe jdoucich kroku, kdy mela bunka maximalni fitness.
     * @param chromosomeSet Sekvence chromozomu urcenych pro jednotlive bunky automatu.
     * @param configurationCount Pocet konfiguraci uvazovanych v procesu evoluce.
     */
    void CheckConfigurations(Automaton::ChromosomeSet &chromosomeSet, int configurationCount);

    /**
     * Zjisti, zda-li uspesnost posloupnosti instrukci v ramci cele mrizky je natolik nizka, za je nutno posloupnosti instrukci resetovat.
     * @param chromosomeSet Sekvence chromozomu urcenych pro jednotlive bunky automatu.
     * @param evolutionStep Aktualni krok vyvojoveho cyklu.
     * @param configurationCount Pocet konfiguraci uvazovanych v procesu evoluce.
     * @return Doporuceni, zda-li se maji posloupnosti instrukci vsech bunek restartovat.
    */
    bool IsRecommendResetGrid(Automaton::ChromosomeSet &chromosomeSet, int evolutionStep, int configurationCount);

    /**
     * V pripade dlouhodobe neuspesnosti posloupnosti instrukci bunky se provede reset teto posloupnosti instrukci.
     * @param chromosomeSet Sekvence chromozomu urcenych pro jednotlive bunky automatu.
     * @param evolutionStep Aktualni krok vyvojoveho cyklu.
     * @param configurationCount Pocet konfiguraci uvazovanych v procesu evoluce.
    */
    void SolveLongtermUnsuccess(Automaton::ChromosomeSet &chromosomeSet, int evolutionStep, int configurationCount);

    /**
     * Vygeneruje posloupnost instrukci pro jednu bunku.
     * @param cellNumber Cislo bunky automatu, ke ktere bude prirazena vygenerovana posloupnost instrukci.
    */
    void GenerateChromosome(int cellNumber);

    /**
     * Vygeneruje sadu pravidel pro kazdou automatu.
     */
    void GenerateChromosomeSet();

    /**
     * Zjisti pocet a posloupnosti instrukci lepe ohodnocenych sousedu aktualni bunky.
     * @param cellNumber Cislo bunky, pro ktere zjistujeme lepe ohodnocene sousedy.
     * @param fitterNeighbour Uloziste pro pozice sousedu s lepe ohodnocenymi posloupnostmi instrukci.
     * @return Pocet sousedu s lepsim fitness.
    */
    int GetFitterNeighbours(int cellNumber, std::vector<int> &fitterNeighbourPosition);

    public:
    EvolutionManagerCP(QObject *parent = 0);
    ~EvolutionManagerCP();

    /**
     * Hleda vhodne chromozomy s instrukcemi k rizeni CA.
     * @param instrList Seznam instrukci potencialne vyuzitelnych k sestaveni chromozomu.
     * @param cellPossibilities Mozne stavy bunek mrizky.
     */
    virtual void FindSolution(std::vector<Automaton::Instruction> instrList);

    /**
     * Ulozi nejuspenejsi chromozom s posloupnostmi instrukci jednotlivych bunek do souboru.
     * @param Cesta k vystupnimu souboru.
     * @return Udava, zda se podarilo ulozit nejuspesnejsi chromozom s posloupnostmi instrukci jednotlivych bunek do souboru.
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
     * Srovna ukazatele na posloupnosti instrukci bunek podle jejich fitness.
     * @param position Vektor s ukazateli na posloupnosti instrukci jednotlivych bunek.
     */
    void SortByFitness(std::vector<int> &position);

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

#endif //EVOLUTION_MANAGER_CP_H_INCLUDED
