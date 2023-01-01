/**
* \file gridWidget.h
* \brief 2D mrizka pro zobrazeni stavu bunek celularniho automatu.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef GRID_WIDGET_H_INCLUDED
#define GRID_WIDGET_H_INCLUDED

#include <QWidget>
#include <iostream>
#include <stdlib.h>
#include "automaton.h"
#include "colorSampler.h"

/// 2D mrizka pro zobrazeni stavu bunek celularniho automatu.
class GridWidget : public QWidget
{
    Q_OBJECT

    Automaton::GridType grid;     ///< Mrizka automatu.
    int actualColorIndex;         ///< Index vybrane barvy.
    int maxSize;                  ///< Maximalni sirka mrizky v px.
    int cellWidth;                ///< Sirka bunky v px.
    QPoint actMousePos;           ///< Aktualni pozice kurzoru mysi.
    bool editableFlag;            ///< Urci, zda-li je mozne mrizku editovat.
    bool mousePressed;            ///< Udava, zda-li je stisknute leve tlacitko mysi.
    bool enabledFlag;             ///< Instrukce k povoleni / zakazu zmen mrizky.
    E_NEIGHBOURHOOD instrNeighbourhood;  ///< Nastavuje pouzivany typ okoli.

    protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);

    public:
    /**
     * Konstruktor.
     * @param parent Rodicovsky objekt.
     * @param maxSize Maximalni sirka mrizky v px.
     * @param colorSender Objekt zasilajici signal o zmene barvy.
     */
    GridWidget(QWidget *parent = 0, int maxSize = 0, QWidget *colorSender = NULL);

    /**
     * Nastavi novou mrizku.
     * @param grid Nova mrizka.
     */
    void SetGrid(Automaton::GridType &grid);

    /**
     * Vynuluje mrizku (neaktivni prazdna plocha).
     */
    void SetEmptyGrid();

    /**
     * Vynuluje mrizku (same mrtve bunky).
     * @param size Sirka automatu.
     */
    void SetEmptyGrid(int size);

    /**
     * Vynuluje mrizku a nastavi ji pro zvoleny typ okoli.
     * @param instrNeighbourhood Typ okoli.
     */
    void SetNeighbourhood(E_NEIGHBOURHOOD instrNeighbourhood);

    /**
     * Vrati aktualni mrizku.
     * @return Aktualni mrizka.
     */
    Automaton::GridType GetGrid();

    /**
     * Povoli / zakaze zmenu mrizky.
     * @param enabled Instrukce k povoleni / zakazu zmen mrizky.
     */
    void SetEnabled(bool enabled);

    private slots:
    /**
     * Zmeni index aktualni barvy.
     * @param colorIndex Index aktualni barvy.
     */
    void ActualColorChanged(int colorIndex);
};

#endif //GRID_WIDGET_H_INCLUDED

