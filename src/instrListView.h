/**
* \file instrListView.h
* \brief Vyberovy box pro ulohu / instrukci.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef INSTR_LIST_VIEW_H_INCLUDED
#define INSTR_LIST_VIEW_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <QListView>
#include <iostream>

/// Vyberovy box pro dvojici 'uloha - instrukce'.
class InstrListView : public QListView
{
    Q_OBJECT

    public:
    InstrListView(QWidget *parent = 0) : QListView(parent) {};

    protected:
    void keyPressEvent(QKeyEvent *event);

    signals:
    /**
     * Signal zaslany po stisknuti tlacitka sipky.
     * @param arrow Smer sipky.
     */
    void arrowKeyPressed(int arrow);
};

#endif // INSTR_LIST_VIEW_H_INCLUDED
