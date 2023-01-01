#include "evolutionPlot.h"

class ConfigScaleDraw : public QwtScaleDraw
{
    public:
    virtual QwtText label(double) const
    {
        QString output = "";
        return output;
    }
};

EvolutionPlot::EvolutionPlot(QWidget *parent, EvolutionManagerGP *evolutionManagerGP, EvolutionManagerCP *evolutionManagerCP) : QwtPlot(parent)
{
    config = Config::GetInstance()->configInstance;

    setAutoReplot(false);
    plotLayout()->setAlignCanvasToScales(true);
    setCanvasLineWidth(1);

    // Nastaveni vychozich hodnot
    actMaxFitness = 0;
    actAvgFitness = 0;
    actEvolutionStep = 0;
    timerInterval = config->timeIntervalEvolutionInstructions;
    timerId = -1;
    timerRunning = false;
    valueCount = 1;
    maxFitness.push_back(0);
    avgFitness.push_back(0);

    realTimeData.assign(config->evolutionGraphValuesCount, 0);

    // Nastaveni legendy
    QwtLegend *legend = new QwtLegend(this);
    insertLegend(legend, QwtPlot::ExternalLegend);
    legend->setGeometry(58, 321, 120, 60);
    setCanvasBackground(Qt::white);

    // Nastaveni osy X
    QwtText helpVariable("Cas");
    helpVariable.setFont(QFont("Helvetica", 9, QFont::Bold));
    setAxisTitle(xBottom, helpVariable);
    setAxisScaleDraw(QwtPlot::xBottom, new ConfigScaleDraw());
    setAxisScale(QwtPlot::xBottom, 0, (config->evolutionGraphValuesCount - 1));

    // Nastaveni osy Y
    helpVariable.setText("Fitness");
    helpVariable.setFont(QFont("Helvetica", 10, QFont::Bold));
    setAxisTitle(yLeft, helpVariable);
    setAxisScale(QwtPlot::yLeft, 0, 100);

    // Nastaveni krivek
    QwtPlotCurve *curve;
    curve = new QwtPlotCurve("Nejlepsi fitness");
    curve->setPen(QPen(Qt::red));
    curve->attach(this);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setBrush(QBrush(Qt::red));
    maxCurve = curve;
    curve = new QwtPlotCurve("Prumerne fitness");
    curve->setPen(QPen(Qt::black));
    curve->setZ(curve->z()+1);
    curve->attach(this);
    avgCurve= curve;
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setBrush(QBrush(Qt::black));

    connect(evolutionManagerGP, SIGNAL(CurrentChromosomeGenerated(float, float)), this, SLOT(ReceiveCurrentChromosomeGenerated(float, float)));
    connect(evolutionManagerCP, SIGNAL(CurrentChromosomeGenerated(float, float)), this, SLOT(ReceiveCurrentChromosomeGenerated(float, float)));
}

void EvolutionPlot::timerEvent(QTimerEvent *)
{
    if(valueCount > config->evolutionGraphValuesCount)
    {   // Prvni hodnota se vyhodi
        maxFitness.erase(maxFitness.begin());
        avgFitness.erase(avgFitness.begin());
    }
    else
    {
        valueCount++;
    }
    maxFitness.push_back(actMaxFitness * 100);       // Na konec se zaradi aktualni hodnota
    avgFitness.push_back(actAvgFitness * 100);
    double maxFitnessArray[config->evolutionGraphValuesCount];     // Prevod na pole
    double avgFitnessArray[config->evolutionGraphValuesCount];
    double timeData[config->evolutionGraphValuesCount];
    for(unsigned int i = 0; i < maxFitness.size(); i++)
    {
        maxFitnessArray[i] = maxFitness[i];
        avgFitnessArray[i] = avgFitness[i];
        timeData[i] = (config->evolutionGraphValuesCount - valueCount) + i;
    }

    maxCurve->setRawData(timeData, maxFitnessArray, valueCount);
    avgCurve->setRawData(timeData, avgFitnessArray, valueCount);

    replot();

    if(!timerRunning)
        killTimer(timerId);
}

void EvolutionPlot::StartTimer()
{
    if(IsTimerRunning())
    {   // Kontrola, zda-li jiz casovac nebezi
        killTimer(timerId);
        timerRunning = false;
    }

    timerRunning = true;
    timerId = startTimer(timerInterval);
}

void EvolutionPlot::KillTimer(bool immidiatelyReaction)
{
    if(IsTimerRunning())
    {
        timerRunning = false;

        if(immidiatelyReaction)
        {   // V pripade pozadovane nezpozdene reakce je timer vypnut ihned (nebude tak zobrazen posledni mezivysledek)
            killTimer(timerId);
        }
    }
}

bool EvolutionPlot::IsTimerRunning()
{
    return timerRunning;
}

void EvolutionPlot::ResetTimer()
{
    valueCount = 0;
    actEvolutionStep = 0;
    maxFitness.clear();
    maxFitness.push_back(0);
    avgFitness.clear();
    avgFitness.push_back(0);
    actMaxFitness = 0;
    actAvgFitness = 0;
    replot();
}

void EvolutionPlot::ReceiveCurrentChromosomeGenerated(float avgCurrentPopulationFitness, float maxCurrentPopulationFitness)
{
    this->actAvgFitness = avgCurrentPopulationFitness;
    this->actMaxFitness = maxCurrentPopulationFitness;
    this->actEvolutionStep++;
}
