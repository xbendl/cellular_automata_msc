/**
* \file evolutionInstructionsTab.h
* \brief Zalozka s evoluci pravidel.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef EVOLUTION_INSTRUCTIONS_TAB_H_INCLUDED
#define EVOLUTION_INSTRUCTIONS_TAB_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "config.h"
#include "instrListView.h"
#include "automaton.h"
#include "evolutionManagerGP.h"
#include "evolutionManagerCP.h"
#include "gridWidget.h"
#include "evolutionPlot.h"

/// Zalozka s evoluci pravidel.
class EvolutionInstructionsTab : public QWidget
{
    Q_OBJECT

    private:
    QTabWidget *tabWidget;

    QLabel *actualFitnessLabel;
    QLabel *actualCellFitnessLabel;
    QLabel *projectStatus;

    QPushButton *evolutionInstructionButton;
    QPushButton *stopEvolutionInstructionButton;
    QPushButton *abortEvolutionInstructionButton;
    QPushButton *saveChromosomeButton;
    QPushButton *addInstructionButton;
    QPushButton *subtractInstructionButton;

    QComboBox *neighbourhoodComboBox;
    QComboBox *taskComboBox;

    QCheckBox *cellularProgrammingCheckbox;

    InstrListView *instructionPool;
    InstrListView *instructionSelection;

    QSpinBox *automatonStepLine;
    QSpinBox *generationCountStepLine;
    QSpinBox *populationSizeStepLine;
    QSpinBox *gridSizeLine;
    QSpinBox *trainSetLine;
    QDoubleSpinBox *mutationProbabilityLine;

    QProgressBar *evoluteChromosomeBar;

    EvolutionPlot *evolutionPlot;              ///< Graf pro zobrazeni fitness automatu.
    EvolutionManagerGP evolutionManagerGP;     ///< Spravce procesu evoluce pravidel (s vyuzitim klasickeho algoritmu genetickeho programovani).
    EvolutionManagerCP evolutionManagerCP;     ///< Spravce procesu evoluce pravidel (s vyuzitim algoritmu celularniho programovani).
    time_t timeSummary;                        ///< Celkovy cas evoluce.
    Config *config;                            ///< Odkaz na instanci tridy s konfiguraci pro zjednoduseni pristupu.
    Automaton::InstructionSetVector instrList; ///< Vektor s posloupnostmi instrukci k rizeni automatu.
    std::vector<Automaton::Instruction> selInstrList;    ///< Vektor s posloupnostmi instrukci k rizeni automatu.

    /**
     * Nastavi prislusne ovladaci prvky udaji z konfiguracniho souboru.
     */
    void SetConfigurationValues();

    public:
    EvolutionInstructionsTab(QTabWidget *tabWidget, QWidget *parent = 0);

    /**
     * Resetuje seznam nazvu instrukci v listboxech (mnozina pripustnych, mnozina vybranych).
     */
    void ResetInstructionListBox();

    protected:
    void paintEvent(QPaintEvent *);

    private slots:
    void TaskIndexChanged();
    void NeighbourhoodIndexChanged();
    void AddInstructionButtonClicked();
    void SubtractInstructionButtonClicked();
    void CellularProgrammingStateChanged();
    void TaskTypeChanged(int);

    /**
     * Zjisti od uzivatele cestu k souboru, do ktereho budou ulozeny instrukcni mnoziny.
     */
    void SetSaveChromosomeFile();

    /**
     * Spusti proces evoluce pravidel pro rizeni CA.
     */
    void StartEvoluteInstructions();

    /**
     * Pozastavi proces evoluce pravidel pro rizeni CA.
     */
    void StopEvoluteInstructions();

    /**
     * Zrusi proces evoluce pravidel.
     */
    void AbortEvoluteInstructions();

    /**
     * Prijme informaci o aktualnim stavu evolucniho vypoctu.
     */
    void ReceiveCurrentChromosomeGenerated();

    /**
     * Prijme informaci o stavu dokonceneho evolucniho vypoctu.
     * @param basicInstructionSet Indikuje, zda-li se byla generovana zakladni sada instrukci nebo sada instrukci pro sebeopravu (pouze v pripade vysetrovani sebe-organizace).
     */
    void ReceiveChromosomeGenerated(bool basicInstructionSet);
};

#endif // EVOLUTION_INSTRUCTIONS_TAB_H_INCLUDED

