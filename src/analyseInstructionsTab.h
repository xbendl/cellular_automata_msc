/**
* \file analyseInstructionsTab.h
* \brief Zalozka s analyzou instrukci.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef ANALYSE_INSTRUCTIONS_TAB_H_INCLUDED
#define ANALYSE_INSTRUCTIONS_TAB_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "config.h"
#include "automaton.h"
#include "automatonCP.h"
#include "automatonGP.h"
#include "evolutionPlot.h"
#include "analyseManager.h"
#include "gridWidget.h"
#include "palette.h"
#include "xmlConfig.h"

/// Zalozka s analyzou pravidel.
class AnalyseInstructionsTab : public QWidget
{
    Q_OBJECT

    private:
    QLabel *automatonRepairmentHeader;
    QLabel *hammingDistanceLabel;
    QLabel *actualStepLabel;
    QLabel *hammingDistanceRepairmentLabel;
    QLabel *actualStepRepairmentLabel;

    QPushButton *automatonStepButton;
    QPushButton *loadFileButton;
    QPushButton *automatonFileButton;
    QPushButton *steppingButton;
    QPushButton *saveResultsButton;
    QPushButton *resetButton;

    QSpinBox *automatonStepLine;
    QSpinBox *steppingLine;
    QDoubleSpinBox *autoDamageLine;
    QCheckBox *damageCheckBox;

    Palette *palette;               //< Paleta pro vyber barev

    bool timerRunning;                            ///< Udava, zda-li bezi casovac.
    int timerId;                                  ///< Identifikator casovace.
    int actualStep;                               ///< Cislo aktualniho kroku.
    Automaton::GridType zygote;                   ///< Tvar zygoty.
    Automaton::GridType pattern;                  ///< Hledany vzor.
    std::vector<int> cellPossibilities;           ///< Mozne stavy bunek automatu.
    GridWidget *grid;                             ///< Mrizka pro automat vyvijejici se dle instrukci z nacteneho chromozomu.
    GridWidget *gridRepairment;                   ///< Mrizka pro automat vyvijejici se dle instrukci z nacteneho chromozomu (se specializovanymi instrukcemi pro sebe-opravu - pouze v pripade ulohy zamerene na sebe-organizaci).
    AnalyseManager analyseManager;                ///< Spravce analyzy pravidel (sledovani automatu po poskozeni).
    Automaton::InstructionSetVector instrList;    ///< Vektor s posloupnostmi instrukci k rizeni automatu.
    Automaton::Chromosome resultChromosome;       ///< Chromozom s instrukcemi k rizeni cinnosti CA.
    Automaton::ChromosomeSet resultChromosomeSet; ///< Mnozina chromozomu s instrukcemi k rizeni cinnosti CA.
    Automaton::ChromosomeSet resultChromosomeSetRepairment;        ///< Mnozina chromozomu s instrukcemi k rizeni cinnosti CA.
    Config *config;                               ///< Odkaz na instanci tridy s konfiguraci pro zjednoduseni pristupu.
    Automaton::GridType whiteGrid;                ///< Pomocna mrizka zaplnena bilymi polickami (uloha majorita / synchronizace).
    Automaton::GridType blackGrid;                ///< Pomocna mrizka zaplnena cernymi polickami (uloha majorita / synchronizace).
    Automaton::GridType requiredGrid;             ///< Pomocna mrizka udrzujici pozadovanou podobu vystupu.
    Automaton::GridType previousGrid;             ///< Pomocna mrizka udrzujici podobu vystupu v predchazejicim kroku.
    Automaton::GridType previousGridRepairment;   ///< Pomocna mrizka udrzujici podobu vystupu v predchazejicim kroku (pri zkoumani schopnosti sebe-opravy pro ulohu sebe-organizace).
    Automaton::HammingDistanceStatistic hammingDistanceStatistic;           ///< Zavislost Hammingovi vzdalenosti na casovem kroku.
    Automaton::HammingDistanceStatistic hammingDistanceRepairmentStatistic; ///< Zavislost Hammingovi vzdalenosti na casovem kroku (pro instrukce trenovane pro ulohu sebe-opravy (uloha sebe-organizace).
    std::vector<Automaton::Point> positionsAdder;           ///< Pozice v mrizce, ktere maji byt porovnavany pri zjistovani fitness (pro ulohu mechanickych vypoctu).

    /**
    * Nastavi prislusne ovladaci prvky udaji z konfiguracniho souboru.
    */
    void SetConfigurationValues();

    /**
    * Nacte instrukce ze souboru.
    * @param inputFile Cesta k vstupniho souboru s instrukcemi.
    * @return Udava, zda se podarilo nacist instrukce ze souboru.
    */
    bool LoadFile(std::string inputFile, E_TASK_TYPE &taskType);

    /**
    * Nacte ulohu ze souboru.
    * @param taskFile Cesta k vstupnimu souboru s ulohou.
    * @return Udava, zda se podarilo nacist ulohu ze souboru.
    */
    bool LoadTask(std::string taskFile);

    /**
    * Ulozi vysledky do souboru.
    * @param outputFile Cesta k vystupnimu souboru.
    * @return Udava, zda se podarilo ulozit vysledky do souboru.
    */
    bool SaveResults(std::string outputFile);

    protected:
    void timerEvent(QTimerEvent *);

    public:
    AnalyseInstructionsTab(QWidget *parent = 0);

    /**
    * Resetuje list instrukci.
    */
    void ResetInstructionList();

    private slots:
    /**
    * Zjisti od uzivatele cestu k souboru s pocatecni konfiguraci automatu.
    */
    void SetOpenAutomatonFile();

    /**
    * Zjisti od uzivatele cestu k souboru s ulozenymi instrukcemi.
    */
    void SetOpenInstructionsFile();

    /**
    * Zjisti od uzivatele cestu k souboru, do ktereho budou ulozeny vysledky.
    */
    void SetSaveResultsFile();

    /**
    * Provede restart ulohy - nastaveni mrizek a citacu do pocatecniho stavu.
    */
    void ResetTask();

    /**
    * Vykona nastaveny pocet kroku automatu.
    */
    void ExecuteSteps();

    /**
    * Prepina proces krokovani vyvoje automatu.
    */
    void ExecuteStepping();

    /**
    * Zobrazi zaslane stavy mrizek automatu.
    * @param grid Aktualni stav mrizky automatu.
    * @param gridRepairment Aktualni stav mrizky automatu rizeneho instrukcemi trenovanymi pro sebe-opravu (v pripade ulohy sebe-organizace).
    * @param lastGrid Udava, zda jsou zasilane stavy mrizky posledni.
    */
    void ShowGridGenerated(Automaton::GridType grid, Automaton::GridType gridRepairment, bool lastGrid);
};

#endif // ANALYSE_INSTRUCTIONS_TAB_H_INCLUDED

