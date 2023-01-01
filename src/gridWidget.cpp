#include <QtGui>
#include <math.h>
#include <iostream>
#include "gridWidget.h"

GridWidget::GridWidget(QWidget *parent, int maxSize, QWidget *colorSender) : QWidget(parent)
{
    this->instrNeighbourhood = NEIGHBOURHOOD_UNKNOWN;
    this->maxSize = maxSize;
    this->editableFlag = false;
    this->actualColorIndex = 0;
    this->cellWidth = 0;
    this->mousePressed = false;
    this->enabledFlag = true;

    // Globalni nastaveni
    setAutoFillBackground(true);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    if(colorSender != NULL)
    {   // Signal o zmene barvy ma smysl prijimat pouze v pripade, ze je mrizka editovatelna
        connect(colorSender, SIGNAL(ActualColorChanged(int)), this, SLOT(ActualColorChanged(int)));
        this->editableFlag = true;
    }
}

void GridWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    // Vykresleni mrizky
    int posX = 0, posY = 0;
    QColor lineColor(20, 20, 20);  // Odstin pro vykresleni delicich car mezi bunkami
    if(grid.size() > 0)
    {   // Mrizka byla inicializovana

        // Vymazani pozadi
        int length = grid.size() * cellWidth;                       // Celkova delka stany automatu v px
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::transparent);
        painter.drawRect(0, 0, (this->width() - 1), (this->height() - 1));

        // Vykresleni delicich car
        painter.setPen(lineColor);
        painter.setBrush(Qt::white);
        for(unsigned int i = 0; i < grid.size() + 1; i++)
        {
            painter.drawLine(QPointF(0, posY), QPointF(length, posY));
            painter.drawLine(QPointF(posX, 0), QPointF(posX, length));
            posX += cellWidth;
            posY += cellWidth;
        }

        // Vykresleni jednotlivych policek
        for(unsigned int row = 0; row < grid.size(); row++)
        {
            for(unsigned int col = 0; col < grid.size(); col++)
            {
                painter.setBrush(ColorSampler::sampler[grid[row][col]]);
                painter.drawRect(col * cellWidth, row * cellWidth, cellWidth, cellWidth);
                if(grid[row][col] == 0)
                {   // Vykresleni otazniku do policky (stav "na barve nezalezi")
                    QFont myFont = QFont("Helvetica", cellWidth / 3, QFont::Bold);
                    QFontMetrics fontMetrics(myFont);
                    painter.setFont(myFont);
                    painter.drawText((col * cellWidth + cellWidth / 3), (row * cellWidth + cellWidth / 2 + fontMetrics.height() / 3 * 1), QString("?"));
                }
            }
        }
    }
    else
    {   // Vykresleni prazdne plochy (po startu programu)
        painter.setPen(Qt::black);
        painter.setBrush(Qt::white);
        painter.drawRect(0, 0, (this->width() - 1), (this->width() - 1));
        painter.drawLine(0, 0, (this->width() - 1), (this->width() - 1));
        painter.drawLine(0, (this->width() - 1), (this->width() - 1), 0);
    }

    // Vykresleni obdelniku okolo policka, nad kterym pohybujeme mysi
    if((grid.size() > 0) && editableFlag && underMouse())
    {   // Kurzor je nad oblasti mrizky

        // Vypocet ohranicujiciho obdelniku
        QRect activeCell = QRect(((actMousePos.x()-1) / cellWidth) * cellWidth, ((actMousePos.y()-1) / cellWidth) * cellWidth, cellWidth, cellWidth);
        QPoint activeCellIndex = QPoint((actMousePos.x()-1) / cellWidth + 1, ((actMousePos.y()-1) / cellWidth) + 1);

        if((this->instrNeighbourhood != NEIGHBOURHOOD_VON_NEUMANN) ||
            (!(activeCellIndex.x() == 1 && activeCellIndex.y() == 1) && !(activeCellIndex.x() == 1 && activeCellIndex.y() == 3) &&
            !(activeCellIndex.x() == 3 && activeCellIndex.y() == 1) && !(activeCellIndex.x() == 3 && activeCellIndex.y() == 3)))
        {
            QPen pen(Qt::magenta, 2);
            painter.setPen(pen);
            painter.setBrush(Qt::transparent);
            painter.drawRect(activeCell);
        }
    }

    // Oznaceni neaktivnich policek pri zadavani instrukce s Von-Neumannovym okolim
    if((grid.size() > 0) && this->instrNeighbourhood == NEIGHBOURHOOD_VON_NEUMANN)
    {
        // Definice pozice neaktivnich policek
        QRect cell1 = QRect(0, 0, cellWidth, cellWidth);
        QRect cell2 = QRect((2 * cellWidth), 0, cellWidth, cellWidth);
        QRect cell3 = QRect(0, (2 * cellWidth), cellWidth, cellWidth);
        QRect cell4 = QRect((2 * cellWidth), (2 * cellWidth), cellWidth, cellWidth);

        // Vykresleni krize pres neaktivni policka
        QPen pen(Qt::black, 1);
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::white);
        painter.drawRect(cell1.left() + 5, cell1.top() + 5, cellWidth - 10, cellWidth - 10);
        painter.drawRect(cell2.left() + 5, cell2.top() + 5, cellWidth - 10, cellWidth - 10);
        painter.drawRect(cell3.left() + 5, cell3.top() + 5, cellWidth - 10, cellWidth - 10);
        painter.drawRect(cell4.left() + 5, cell4.top() + 5, cellWidth - 10, cellWidth - 10);
        painter.setPen(pen);
        painter.drawLine(cell1.left(), cell1.top(), cell1.right(), cell1.bottom());
        painter.drawLine(cell1.left(), cell1.bottom(), cell1.right(), cell1.top());
        painter.drawLine(cell2.left(), cell2.top(), cell2.right(), cell2.bottom());
        painter.drawLine(cell2.left(), cell2.bottom(), cell2.right(), cell2.top());
        painter.drawLine(cell3.left(), cell3.top(), cell3.right(), cell3.bottom());
        painter.drawLine(cell3.left(), cell3.bottom(), cell3.right(), cell3.top());
        painter.drawLine(cell4.left(), cell4.top(), cell4.right(), cell4.bottom());
        painter.drawLine(cell4.left(), cell4.bottom(), cell4.right(), cell4.top());
    }
}

void GridWidget::enterEvent(QEvent *)
{
    if(this->editableFlag)
        setCursor(Qt::PointingHandCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void GridWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    actMousePos.setX(mouseEvent->x());
    actMousePos.setY(mouseEvent->y());

    if((grid.size() > 0) && editableFlag && mousePressed && (mouseEvent->x() > 0) && (mouseEvent->x() < this->width()) && (mouseEvent->y() > 0) && (mouseEvent->y() < this->height()))
    {   // Drzi-li uzivatel leve tlacitko mysi, pak je bunka vyplnena aktualni vybranou barvou

        QPoint activeCellIndex = QPoint((actMousePos.x()-2) / cellWidth + 1, ((actMousePos.y()-2) / cellWidth) + 1);
        if((this->instrNeighbourhood != NEIGHBOURHOOD_VON_NEUMANN) ||
            (!(activeCellIndex.x() == 1 && activeCellIndex.y() == 1) && !(activeCellIndex.x() == 1 && activeCellIndex.y() == 3) &&
            !(activeCellIndex.x() == 3 && activeCellIndex.y() == 1) && !(activeCellIndex.x() == 3 && activeCellIndex.y() == 3)))
            grid[(mouseEvent->y() - 2) / cellWidth][(mouseEvent->x() - 2) / cellWidth] = actualColorIndex;

    }

    update();
}

void GridWidget::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void GridWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
    mousePressed = true;

    if((grid.size() > 0) && editableFlag)
    {   // Mrizka byla inicializovana

        QPoint activeCellIndex = QPoint((actMousePos.x()-1) / cellWidth + 1, ((actMousePos.y()-1) / cellWidth) + 1);
        if((this->instrNeighbourhood != NEIGHBOURHOOD_VON_NEUMANN) ||
            (!(activeCellIndex.x() == 1 && activeCellIndex.y() == 1) && !(activeCellIndex.x() == 1 && activeCellIndex.y() == 3) &&
            !(activeCellIndex.x() == 3 && activeCellIndex.y() == 1) && !(activeCellIndex.x() == 3 && activeCellIndex.y() == 3)))
            grid[(mouseEvent->y() - 2) / cellWidth][(mouseEvent->x() - 2) / cellWidth] = actualColorIndex;
        update();
    }
}

void GridWidget::leaveEvent(QEvent *)
{
    update();
}

void GridWidget::SetEmptyGrid()
{
    Automaton::GridType emptyGrid;
    grid = emptyGrid;

    update();
}

void GridWidget::SetEmptyGrid(int size)
{
    // Vynulovani mrizky
    Automaton::CellVector emptyCellVector;
    emptyCellVector.assign(size, (Automaton::CellType)0);

    grid.assign(size, emptyCellVector);

    // Nastaveni sirky mrizky
    int newLength = grid.size() * ((maxSize - 1) / grid.size()) + 1;
    setFixedSize(newLength, newLength);

    // Nastaveni sirky bunky
    this->cellWidth = floor((this->width() - 1) / grid.size());

    update();
}

void GridWidget::SetNeighbourhood(E_NEIGHBOURHOOD instrNeighbourhood)
{
    this->instrNeighbourhood = instrNeighbourhood;
}

void GridWidget::SetGrid(Automaton::GridType &pattern)
{
    this->grid = pattern;
    int newLength = grid.size() * ((maxSize - 1) / grid.size()) + 1;
    setFixedSize(newLength, newLength);

    // Nastaveni sirky bunky
    this->cellWidth = floor((this->width() - 1) / grid.size());

    update();
}

void GridWidget::ActualColorChanged(int colorIndex)
{
    actualColorIndex = colorIndex;
}

Automaton::GridType GridWidget::GetGrid()
{
    return grid;
}

void GridWidget::SetEnabled(bool enabled)
{
    editableFlag = enabled;
}

