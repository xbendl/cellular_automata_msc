/**
* \file analyseManager.h
* \brief Spravce analyzy pravidel.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef ANALYSE_MANAGER_H_INCLUDED
#define ANALYSE_MANAGER_H_INCLUDED
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sstream>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMetaType>
#include "config.h"
#include "automaton.h"
#include "automatonGP.h"
#include "automatonCP.h"

/// Spravce analyzy pravidel.
class AnalyseManager : public QThread
{
    Q_OBJECT

    private:
    Automaton::GridType zygote;                ///< Zygota.
    Automaton::GridType pattern;               ///< Hledany vzor
    bool abortFlag;                            ///< Priznak ukonceni evolucnich vypoctu.
    bool sleepFlag;                            ///< Priznak pozastaveni evolucnich vypoctu.
    Automaton *automaton;                      ///< Vyvijeny automat.
    Automaton *automatonRepairment;            ///< Vyvijeny automat (verze s posloupnostmi instrukci trenovanymi pro sebe-opravu v pripade ulohy sebe-organizace).
    std::vector<int> cellPossibilities;        ///< Mozne stavy bunek automatu.
    int executeCycles;                         ///< Pocet prechodu automatu, ktere maji byt provedeny.
    Automaton::Chromosome resultChromosome;    ///< Chromozom s instrukcemi k rizeni cinnosti CA.
    Automaton::ChromosomeSet resultChromosomeSet;           ///< Sekvence chromozomu s instrukcemi pro jednotlive bunky k rizeni cinnosti CA.
    Automaton::ChromosomeSet resultChromosomeSetRepairment; ///< Sekvence chromozomu s instrukcemi pro jednotlive bunky k rizeni cinnosti CA (instrukce trenovane pro sebe-opravu v pripade ulohy sebe-organizace).
    Automaton::InstructionSetVector instrList; ///< Vektor s posloupnostmi instrukci k rizeni automatu.
    double damageProbability;                  ///< Pravdepodobnost poskozeni.
    Config *config;                            ///< Odkaz na instanci tridy s konfiguraci pro zjednoduseni pristupu

    QMutex mutex;
    QWaitCondition condition;

    public:
    AnalyseManager(QObject *parent = 0);

    /**
     * Destruktor.
     */
    ~AnalyseManager();

    /**
     * Smycka evolucniho vypoctu.
     */
    void run();

    /**
     * Sleduje chovani automatu pri vyvoji podle instrukci chromozomu (pro klasicky algoritmus genetickeho programovani).
     * @param zygote Zygota.
     * @param grid Hledany vzor.
     * @param resultChromosome Chromozom s instrukcemi k rizeni cinnosti CA.
     * @param cellPossibilities Mozne stavy bunek automatu.
     */
    void ObserveAutomaton(Automaton::GridType zygote, Automaton::GridType pattern, Automaton::Chromosome &resultChromosome, std::vector<int> cellPossibilities);

    /**
     * Sleduje chovani automatu pri vyvoji podle mnoziny instrukci chromozomu (pro algoritmus celularniho programova).
     * @param zygote Zygota.
     * @param grid Hledany vzor.
     * @param resultChromosomeSet Mnozina chromozomu s instrukcemi k rizeni cinnosti CA.
     * @param resultChromosomeSetRepairment Mnozina chromozomu s instrukcemi k rizeni cinnosti CA (instrukce trenovane pro sebe-opravu v pripade ulohy sebe-organizace).
     * @param cellPossibilities Mozne stavy bunek automatu.
     */
    void ObserveAutomaton(Automaton::GridType zygote, Automaton::GridType pattern, Automaton::ChromosomeSet &resultChromosomeSet, Automaton::ChromosomeSet &resultChromosomeSetRepairment, std::vector<int> cellPossibilities);

    /**
     * Provede stanoveny pocet prechodu sledovanych automatu.
     * @param grid Mrizka vyvijeneho automatu.
     * @param gridRepairment Mrizka vyvijeneho automatu podle instrukci trenovanych pro sebeopravu (v pripade ulohy sebe-organizace).
     * @param cyclesCount Pocet cyklu, ktere maji byt provedeny.
     * @param damageProbability Mira poskozeni v kazdem kroku vyvoje.
     */
    void SetCycles(Automaton::GridType grid, Automaton::GridType gridRepairment, int cyclesCount, double damageProbability);

    /**
     * Vytiskne zadany chromozom.
     * @param chromosome Zadany chromozom.
    * @return Vypis zadane instrukci zadaneho chromozomu.
     */
    std::string PrintChromosome(Automaton::Chromosome &chromosome);

    /**
     * Vytiskne zadanou mnozinu chromozomu.
     * @param chromosomeSet Zadana mnozina chromozomu.
     * @return Vypis zadane instrukci zadane mnoziny chromozomu.
     */
    std::string PrintChromosomeSet(Automaton::ChromosomeSet &chromosomeSet);

    signals:
    /**
     * Signal zaslany po vykonani jednoho kroku sledovanych automatu.
     * @param grid Mrizka automatu.
     * @param gridRepairment Mrizka automatu rizeneho posloupnostmi instrukci trenovanych pro sebe-opravu (v pripade ulohy sebe-organizace).
     * @param lastGrid Udava, zda jsou zasilane stavy mrizky posledni.
     */
    void GridGenerated(Automaton::GridType grid, Automaton::GridType gridRepairment, bool lastGrid);
};

#endif // ANALYSE_MANAGER_H_INCLUDED

