/**
* \file taskTab.h
* \brief Zalozka s tvorbou novych uloh.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef TASK_TAB_H_INCLUDED
#define TASK_TAB_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "automaton.h"
#include "gridWidget.h"
#include "palette.h"

/// Zalozka s tvorbou novych uloh.
class TaskTab : public QWidget
{
    Q_OBJECT

    private:
    QLabel *actualFitnessLabel;
    QLabel *actualCellFitnessLabel;

    QPushButton *newTaskButton;
    QPushButton *loadTaskButton;
    QPushButton *saveTaskButton;

    GridWidget *zygoteGridWidget;   ///< Mrizka pro zobrazeni zygoty.
    GridWidget *patternGridWidget;  ///< Mrizka pro zobrazeni hledaneho vzoru.

    Palette *palette;               ///< Paleta pro vyber barev.

    /**
     * Nacte ulohu ze souboru.
     * @param inputFile Cesta k vstupniho souboru.
     * @param zygoteGrid Nactena zygota.
     * @param patternGrid Nacteny hledany vzor.
     * @return Udava, zda se podarilo nacist ulohu ze souboru.
     */
    bool LoadTask(std::string inputFile, Automaton::GridType &zygoteGrid, Automaton::GridType &patternGrid);

    /**
     * Ulozi ulohu do souboru.
     * @param outputFile Cesta k vystupnimu souboru.
     * @return Udava, zda se podarilo ulozit ulohu do souboru.
     */
    bool SaveTask(std::string outputFile);

    public:
    /**
     * Konstruktor.
     */
    TaskTab(QWidget *parent = 0);

    private slots:
    /**
     * Zjisti od uzivatele cestu k souboru s ulohou.
     */
    void SetOpenTaskFile();

    /**
     * Zjisti od uzivatele cestu k souboru, do ktereho bude ulozena vytvorena uloha.
     */
    void SetSaveTaskFile();

    /**
     * Zjisti od uzivatele delku strany automatu.
     */
    void SetNewTask();
};

#endif // TASK_TAB_H_INCLUDED
