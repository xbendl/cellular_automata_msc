/**
* \file instrNeighDialog.h
* \brief Dialog pro zadani typu nove instrukce (5okoli / 9okoli).
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef INSTR_NEIGH_DIALOG_H_INCLUDED
#define INSTR_NEIGH_DIALOG_H_INCLUDED

#include <QtGui>
#include <QDialog>
#include <QWidget>
#include <iostream>
#include "automaton.h"

/// Dialog pro zadani nove instrukce (5okoli / 9okoli).
class InstrNeighDialog : public QDialog
{
    Q_OBJECT

    QComboBox *neighbourhoodComboBox;

    public:
    InstrNeighDialog(E_NEIGHBOURHOOD defaultNeighbourhood, QWidget *parent = 0);

    /**
     * Ziska typ instrukce (5okoli / 9okoli).
     * @return Typ instrukce (5okoli / 9okoli).
     */
    E_NEIGHBOURHOOD GetInstructionType();
};


#endif // INSTR_NEIGH_DIALOG_H_INCLUDED
