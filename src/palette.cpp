#include "palette.h"

using namespace std;

Palette::Palette(QWidget *parent, int width, int height) : QWidget(parent)
{
    cellPossibilities = (ColorSampler::sampler).size();

    actualColorIndex = 0;
    colorCellHeight = 20;

    setFixedSize(width, height);
    colorCellWidth = floor((width - 1) / (ColorSampler::sampler).size()); // Sirka policka s barvou

    colorBox.moveTo(0, 0);                                                      // Obdelnik s vyberem barev
    colorBox.setSize(QSize(((ColorSampler::sampler).size() * colorCellWidth), colorCellHeight));

    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
}

void Palette::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    // Vykresleni boxu pro vyber barev
    for(int colorIndex = 0; colorIndex < (int)(ColorSampler::sampler).size(); colorIndex++)
    {
        painter.setPen(Qt::transparent);
        painter.setBrush(ColorSampler::sampler[colorIndex]);
        painter.drawRect((colorIndex * colorCellWidth), 0, colorCellWidth, colorCellHeight);
        if(colorIndex >= cellPossibilities)
        {
            painter.setPen(QPen(Qt::magenta));
            painter.drawLine((colorIndex * colorCellWidth), 0, (colorIndex * colorCellWidth) + colorCellWidth, colorCellHeight);
            painter.drawLine((colorIndex * colorCellWidth), colorCellHeight, (colorIndex * colorCellWidth) + colorCellWidth, 0);
        }
    }
    // Vykresleni otazniku ("na barve nezalezi") na prvni policko palety
    painter.setPen(Qt::black);
    painter.drawText(5, 15, QString("?"));
    painter.drawLine((colorCellWidth - 1), 0, (colorCellWidth - 1), colorCellHeight);

    // Vykresleni mrizky okolo boxu pro vyber barev
    painter.setPen(Qt::black);
    painter.setBrush(Qt::transparent);
    painter.drawRect(colorBox);

    // Vykresleni aktualni barvy
    painter.setPen(QPen(Qt::magenta, 4));
    painter.drawRect(actualColorIndex * colorCellWidth, 1, (colorCellWidth - 2), (colorCellHeight - 2));
}

void Palette::mousePressEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->x() < (cellPossibilities * colorCellWidth))
    {
        actualColorIndex = mouseEvent->x() / colorCellWidth;
        emit ActualColorChanged(actualColorIndex);   // Signal informujici o zmene aktualni barvy
        update();
    }
}

void Palette::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->x() < (cellPossibilities * colorCellWidth))
        setCursor(Qt::PointingHandCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void Palette::SetPaletteRange(int argCellPossibilities)
{
    cellPossibilities = argCellPossibilities;

    if(cellPossibilities > ((int)(ColorSampler::sampler).size()))
        cellPossibilities = (ColorSampler::sampler).size();

    actualColorIndex = 0;

    update();
}

void Palette::SetColorIndex(int colorIndex)
{
    actualColorIndex = colorIndex;
    update();
}

