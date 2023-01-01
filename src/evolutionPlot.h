/**
* \file evolutionPlot.h
* \brief Graf vyvoje fitness
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef EVOLUTION_PLOT
#define EVOLUTION_PLOT

#include <stdlib.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include "evolutionManagerCP.h"
#include "evolutionManagerGP.h"
#include "config.h"

/// Spravce diagramu zobrazujiciho vyvoj fitness.
class EvolutionPlot : public QwtPlot
{
    Q_OBJECT

    public:
    EvolutionPlot(QWidget *parent, EvolutionManagerGP *evolutionManagerGP, EvolutionManagerCP *evolutionManagerCP);

    /**
     * Spusti casovac.
     */
    void StartTimer();

    /**
     * Vypne casovac.
     * @param immidiatelyReaction Urci rychlost reakce (zda ma dobehnout aktualni krok).
     */
    void KillTimer(bool immidiatelyReaction);

    /**
     * Vynuluje nastaveni casovace.
     */
    void ResetTimer();

    /**
     * Zjisti stav casovace.
     * @return Stav casovace.
     */
    bool IsTimerRunning();

    protected:
    void timerEvent(QTimerEvent *timerEvent);

    private:
    Config *config;                       ///< Odkaz na instanci tridy s konfiguraci pro zjednoduseni pristupu.
    int helping;
    bool timerRunning;
    int valueCount;     ///< Pocet doposud uchovanych hodnot.
    int maxValueCount;  ///< Maximalni pocet uchovavanych hodnot.
    int timerInterval;  ///< Interval tiku casovace.
    int timerId;        ///< ID casovace.
    double actMaxFitness;      ///< Aktualni nejlepsi fitness.
    double actAvgFitness;      ///< Aktualni prumerne fitness.
    double actEvolutionStep;   ///< Aktualni evolucni krok.
    std::vector<double> maxFitness;  ///< Hodnoty pro krivku zobrazujici nejlepsi fitness.
    std::vector<double> avgFitness;  ///< Hodnoty pro krivku zobrazujici prumerny fitness.
    std::vector<int> realTimeData;   ///< Cisla evolucnich kroku zobrazovane na ose X.
    QwtPlotCurve *maxCurve;          ///< Ukazatel na krivku zobrazujici nejlepsi fitness.
    QwtPlotCurve *avgCurve;          ///< Ukazatel na krivku zobrazujici prumerny fitness.

    private slots:
    /**
     * Prijme informaci o aktualnim stavu evolucniho vypoctu.
     * @param avgCurrentPopulationFitness Prumerne reseni v aktualnim evolucnim kroku.
     * @param maxCurrentPopulationFitness Nejlepsi reseni v aktualnim evolucnim kroku.
     */
    void ReceiveCurrentChromosomeGenerated(float avgCurrentPopulationFitness, float maxCurrentPopulationFitness);
};

#endif // EVOLUTION_PLOT

