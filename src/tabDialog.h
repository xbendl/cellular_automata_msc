/**
* \file tabDialog.h
* \brief Spravce hlavniho okna.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef TAB_DIALOG_H
#define TAB_DIALOG_H

#include <QtGui>
#include <QDialog>
#include <QTabWidget>
#include <stdlib.h>
#include "evolutionInstructionsTab.h"
#include "analyseInstructionsTab.h"
#include "instructionTab.h"
#include "taskTab.h"
#include "config.h"

/// Spravce hlavniho okna.
class TabDialog : public QDialog
{
    Q_OBJECT

    private:
    QTabWidget *tabWidget;
    EvolutionInstructionsTab *evolutionInstructionsTab;
    AnalyseInstructionsTab *analyseInstructionsTab;
    TaskTab *taskTab;
    InstructionTab *instructionTab;

    public:
    TabDialog(QWidget *parent = 0);

    void SetTabWidgetEnabled(bool enabledFlag);

    private slots:
    void TabIndexChanged(int index);
};

#endif // TAB_DIALOG_H

