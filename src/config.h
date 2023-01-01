/**
* \file config.h
* \brief Spravce konfiguracnich udaju.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <algorithm>

/// Vycet moznych uloh k reseni.
enum E_TASK_TYPE
{
    TASK_MAJORITY = 0,          ///< Uloha majority (density task, majority task).
    TASK_SYNCHRONIZATION = 1,   ///< Uloha synchronizace (synchronization task).
    TASK_SELF_ORGANIZATION = 2, ///< Uloha sebe-organizace.
    TASK_COMPUTATION_ADDER = 3, ///< Uloha vypoctu (uplna jednobitova scitacka).
    TASK_UNKNOWN = 4,           ///< Neznama uloha.
    TASK_COUNT = 5              ///< Pocet variant uloh.
};

/// Spravce konfiguracnich udaju.
class Config
{
    private:
    Config();
    ~Config();

    public:
    int populationSize;                 ///< Velikost populace.
    int generationCount;                ///< Pocet generaci.
    int neighbourhood;                  ///< Celkovy pocet prvku ovlivnujici nasledujici stav bunky.
    int tournamentSize;                 ///< Pocet ucastniku turnajove selekce.
    int elitismSize;                    ///< Pocet jedincu s nejlepsim fitness automaticky zarazovanych do nove populace.
    int logInterval;                    ///< Pocet evolucnich kroku, po kterych se zaznamena statistika k aktualni populaci.
    int automatonStep;                  ///< Pocet kroku CA behem jednoho vyvojoveho kroku.
    int evolutionGraphValuesCount;      ///< Pocet hodnot na grafu vyvoje fitness pri evoluci pravidel.
    int defaultStepInterval;            ///< Pocet kroku nabizeny pro preskoceni pri analyze pravidel.
    int gridSize;                       ///< Rozmer mrizky vzorku z trenovaci mnoziny.
    int trainSetSize;                   ///< Pocet vzorku v trenovaci mnozine.
    int instructionApplication;         ///< Typ aplikace instrukci (0 = aplikace vsech vyhovujici, 1 = aplikace prvni vyhovujici)
    int damagedConfigurationCount;      ///< Pocet zamerne poskozenych konfiguraci odvozenych od hledane konfigurace, ktere budou pouzity pro trenovani schopnosti sebeopravy
    double maxDamagedPercentage;        ///< Maximalni mozne procentualni poskozeni vzoru pri trenovani schopnosti sebeopravy
    int rulesetCount;                   ///< Pocet ruznych sad pravidel pro rizeni automatu (pro kvazi-uniformni variantu).
    double nopProbability;              ///< Probability of placing NOP (no-operation) instruction into chromosome instead of regular instruction.
    double defaultDamage;               ///< Velikost poskozeni nabizena defaultne pri analyze pravidel.
    E_TASK_TYPE task;                   ///< Typ resene ulohy (majorita / synchronizace).
    double mutationProbability;         ///< Pravdepodobnost mutace.
    double timeIntervalEvolutionInstructions;  ///< Interval pro pridani nove hodnoty do grafu vyvoje fitness pri evoluci pravidel.
    double timeIntervalAnalyseInstructions;    ///< Interval mezi prechody automatu behem jeho analyzy.
    std::string inputAnalyseFile;       ///< Cesta ke vstupnimu souboru s chromozomem s posloupnosti instrukci k rizeni cinnosti GA.
    std::string inputAnalyseFile2;      ///< Cesta ke vstupnimu souboru s konkretni pocatecni konfiguraci.
    int instructionCount;               ///< Pocet instrukci v ramci chromozomu.
    static Config *configInstance;      ///< Reference na instanci objektu.
    std::string configFile;             ///< Cesta ke konfiguracnimu souboru.
    std::string taskFile;               ///< Cesta k souboru s definici ulohy (vyzadovane u ulohy type sebe-organizace).
    std::string xmlFile;                ///< Cesta k XML souboru s ulozenymi instrukcemi.

    // Cellular programming parameters
    bool cellularProgramming;           ///< Povoleni pouziti algoritmu celularniho programovani (0 = nepovoleno, 1 = povoleno).
    int lockStepCount;                  ///< Pocet po sobe jdoucich evolucnich kroku, ve kterych musi byt fitness bunky maximalni mozne, po kterych se dosahne zamknuti pravidel bunky.
    int initialPhaseStep;               ///< Pocet kroku po startu simulace, ve kterych nemohou byt posloupnosti instrukci bunek restartovany.
    int fitnessMeasuringSequence;       ///< Delka sekvence hodnot fitness branych v potaz pri vypoctu uspesnosti posloupnosti instrukci.
    bool respectFitnessImprovement;     ///< Udava, ma-li byt respektovana puvodni posloupnost instrukci bunky, zvysi-li se po vyvojovem kroku jeji fitness (prestoze stale existuji sousede s vyssim fitness).
    double resetGridPercentageLimit;    ///< Procentualni vyjadreni limitu pro pocet bunek, ktere musi byt v cilovem stavu, aby nedoslo k resetu vsech pravidel mrizky.
    double resetCellPercentageLimit;    ///< Procentualni vyjadreni uspesnosti instrukci bunky, ktera je nutne k tomu, aby nedoslo k jejimu resetu.
    bool extendedInstructionCalculation;       ///< Udava, zda se pro vypocet nove posloupnosti instrukci pouziji posloupnosti instrukci sousedicich 4 bunek (false) nebo 8 bunek (true)
    bool boundaryType;                  ///< Typ okrajových podmínek používaný při výpočtu nového stavu u krajních buněk mřížky automatu (konstantní / cyklické).
    bool rotationInstruction;           ///< Povoleni modifikace zpusobu aplikace instrukci zpocivajici v postupne radkove zamene pri neuspesne aplikaci (0 = disabled, 1 = enabled).

    /**
     * Nacte konfiguracni udaje z konfiguracniho souboru.
     * @return Udava, zda-li se nacteni konfiguracnich udaju podarilo.
     */
    bool LoadConfiguration();

    /**
     * Vrati referenci na instanci objektu (pokud neexistuje, vytvori ji).
     * @return Reference na instanci objektu.
     */
    static Config *GetInstance()
    {
        if(!configInstance)
            configInstance = new Config;

        return configInstance;
    }
};

#endif // CONFIG_H_INCLUDED

