#include "instrListView.h"

using namespace std;

void InstrListView::keyPressEvent(QKeyEvent *event)
{
    QListView::keyPressEvent(event);

    int keyCode = event->key();
    if((keyCode == Qt::Key_Up) || (keyCode == Qt::Key_Down) || (keyCode == Qt::Key_Right) || (keyCode == Qt::Key_Left))
        emit arrowKeyPressed(keyCode);
}
