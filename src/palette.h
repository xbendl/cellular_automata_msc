/**
* \file palette.h
* \brief Paleta barev reprezentujici stavy bunek automatu.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "colorSampler.h"

/// Paleta barev reprezentujici stavy bunek automatu.
class Palette : public QWidget
{
    Q_OBJECT

    private:
    QRect colorBox;            ///< Obdelnik ohranicujici box s barvami.
    int colorCellWidth;        ///< Sirka policka s barvou.
    int colorCellHeight;       ///< Vyska policka s barvou.
    int actualColorIndex;      ///< Index aktualni vybrane barvy.
    int cellPossibilities;     ///< Pocet moznych barev na palete pro zvolenou ulohu.

    protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);

    public:
    Palette(QWidget *parent = 0, int width = 0, int height = 0);

    /**
     * Nastavi pouzitelne barvy na palete.
     * @param cellPossibilities Pocet pouzitelnych barev.
     */
    void SetPaletteRange(int cellPossibilities);

    /**
     * Zmeni vybranou barvu na palete.
     * @param colorIndex Index vybrane barvy.
     */
    void SetColorIndex(int colorIndex);

    signals:
    /**
     * Signal zaslany po zmene aktualni barvy.
     * @param colorIndex Index aktualni barvy.
     */
    void ActualColorChanged(int colorIndex);
};

#endif // PALETTE_H_INCLUDED


