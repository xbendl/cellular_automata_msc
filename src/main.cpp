#include <QtGui>
#include <time.h>
#include <iostream>
#include "tabDialog.h"
#include "colorSampler.h"

using namespace std;

std::vector<QColor> ColorSampler::sampler;

int main(int argc, char *argv[])
{
    // Inicializace barev
    ColorSampler::AddColor(Qt::white);
    ColorSampler::AddColor(Qt::white);
    ColorSampler::AddColor(Qt::gray);
    ColorSampler::AddColor(Qt::black);
    ColorSampler::AddColor(Qt::red);
    ColorSampler::AddColor(Qt::darkRed);
    ColorSampler::AddColor(Qt::green);
    ColorSampler::AddColor(Qt::blue);
    ColorSampler::AddColor(Qt::yellow);

    srand(time(NULL));

    // Start aplikace
    QApplication app(argc, argv);
    TabDialog tabDialog;
    tabDialog.show();

    return app.exec();
}

