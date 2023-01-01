/**
* \file instructionTab.h
* \brief Zalozka s tvorbou novych instrukci.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef INSTRUCTION_TAB_H
#define INSTRUCTION_TAB_H

#include <QtGui>
#include <QDialog>
#include <QWidget>
#include <QListView>
#include <QComboBox>
#include <QInputDialog>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>
#include "config.h"
#include "automaton.h"
#include "gridWidget.h"
#include "palette.h"
#include "xmlConfig.h"
#include "instrNeighDialog.h"
#include "instrListView.h"

/// Zalozka s tvorbou novych instrukci.
class InstructionTab : public QDialog
{
    Q_OBJECT

    private:
    QLabel *actualFitnessLabel;
    QLabel *actualCellFitnessLabel;

    QPushButton *newInstructionButton;
    QPushButton *deleteInstructionButton;
    QPushButton *saveInstructionButton;

    InstrListView *instructionListView;
    InstrListView *taskListView;

    QComboBox *neighbourhoodComboBox;

    GridWidget *neighbourhoodGridWidget;  ///< Mrizka pro zobrazeni okoli pro nastaveni instrukce.
    GridWidget *outputGridWidget;         ///< Mrizka pro zobrazeni vystupu pro nastaveni instrukce.

    Palette *palette;                     ///< Paleta pro vyber barev.

    int lastId;                           ///< Posledni pouzity identifikator instrukce v XML (dulezite pro generovani noveho identifikatoru).
    Config *config;                       ///< Odkaz na instanci tridy s konfiguraci pro zjednoduseni pristupu.

    Automaton::InstructionSetVector instructionList; ///< Vektor s posloupnostmi instrukci k rizeni automatu.
    std::vector<int> cellPossibilitiesCount;        ///< Pocet moznych stavu bunky automatu pro jednotlive ulohy.

    /**
     * Aktualizuje seznam nazvu instrukci.
     * @param taskIndex Index ulohy.
     * @param neighbourhoodIndex Typ okoli.
     */
    void ActualizeInstructionList(int taskIndex, int neighbourhoodIndex);

    /**
     * Nacte ulohu ze souboru.
     * @param inputFile Cesta k vstupniho souboru.
     * @param grid Nactena vstupni konfigurace.
     * @return Udava, zda se podarilo nacist ulohu ze souboru.
     */
    bool LoadTask(std::string inputFile, Automaton::GridType &inputGrid);

    /**
     * Sestavi mrizku podle zadane instrukce.
     * @param configuration Konfigurace mrizky vyjadrena kodem - cislem.
     * @return Vytvorena mrizka
     */
    Automaton::GridType CreateGrid(Automaton::CellType configuration[]);

    public:
    InstructionTab(QWidget *parent = 0);

    protected slots:
    /**
     * Zareaguje na zaslani informace o stisknuti sipky v ramci listboxu - seznamu uloh
     * @param keyCode Kod stisknute sipky (horni, dolni, leva, prava)
     */
    void TaskListViewArrowKeyPressed(int keyCode);

    /**
     * Zareaguje na zaslani informace o stisknuti sipky v ramci listboxu - seznamu instrukce
     * @param keyCode Kod stisknute sipky (horni, dolni, leva, prava)
     */
    void InstrListViewArrowKeyPressed(int keyCode);
    /**
     * Zareaguje na vyber ulohy (zajisti zobrazeni vhodnych instrukci).
     * @param taskIndex
     */
    void TaskListViewEvent(const QModelIndex &taskIndex);

    /**
     * Zareaguje na vyber instrukce (zajisti zobrazeni v nahledove mrizce).
     * @param instrIndex
     */
    void InstructionListViewEvent(const QModelIndex &instrIndex);

    /**
     * Zareaguje na zmenu typu okoli (zajisti zmenu instrukci ve vyberu).
     * @param neighbourhoodIndex Vybrany typ okoli.
     */
    void NeighbourhoodComboBoxEvent(int neighbourhoodIndex);

    /**
     * Zjisti od uzivatele novou instrukci.
     */
    void CreateInstruction();

    /**
     * Odstran existujici instrukce
     */
    void DeleteInstruction();

    /**
     * Uloz novou instrukci do souboru.
     */
    void SaveInstruction();
};

#endif // INSTRUCTION_TAB_H
