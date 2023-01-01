#include "analyseManager.h"

using namespace std;

AnalyseManager::AnalyseManager(QObject *) : QThread()
{
    automaton = NULL;
    automatonRepairment = NULL;

    // Nastaveni priznaku ukonceni a uspani
    abortFlag = false;
    sleepFlag = true;

    executeCycles = 0;
    damageProbability = 0;

    config = Config::GetInstance()->configInstance;
}

AnalyseManager::~AnalyseManager()
{
    // Ukonceni vlakna
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();
    wait();                // Pockani na dobehnuti vlakna

    // Dealokace zabrane pameti
    if(automaton != NULL)
        delete automaton;
    if(automatonRepairment != NULL)
        delete automatonRepairment;
}

void AnalyseManager::ObserveAutomaton(Automaton::GridType zygote, Automaton::GridType pattern, Automaton::Chromosome &resultChromosome, std::vector<int> cellPossibilities)
{
    this->zygote = zygote;
    this->pattern = pattern;
    this->resultChromosome = resultChromosome;
    this->cellPossibilities = cellPossibilities;

    if(automaton != NULL)
        delete automaton;

    automaton = new AutomatonGP(zygote, this->resultChromosome, (E_NEIGHBOURHOOD)config->neighbourhood);

    // Spusteni automatu
    if(!isRunning())
        start();
}

void AnalyseManager::ObserveAutomaton(Automaton::GridType zygote, Automaton::GridType pattern, Automaton::ChromosomeSet &resultChromosomeSet, Automaton::ChromosomeSet &resultChromosomeSetRepairment, std::vector<int> cellPossibilities)
{
    this->zygote = zygote;
    this->pattern = pattern;
    this->resultChromosomeSet = resultChromosomeSet;
    this->resultChromosomeSetRepairment = resultChromosomeSetRepairment;
    this->cellPossibilities = cellPossibilities;

    if(automaton != NULL)
        delete automaton;
    if(automatonRepairment != NULL)
        delete automatonRepairment;

    automaton = new AutomatonCP(zygote, this->resultChromosomeSet, (E_NEIGHBOURHOOD)config->neighbourhood);
    if(!this->resultChromosomeSetRepairment.empty())
        automatonRepairment = new AutomatonCP(zygote, this->resultChromosomeSetRepairment, (E_NEIGHBOURHOOD)config->neighbourhood);

    // Spusteni automatu
    if(!isRunning())
        start();
}

void AnalyseManager::run()
{
    // Smycka provadejici prechody CA
    while(true)
    {
        // Uspani po provedeni prechodu
        if(executeCycles <= 0)
        {
            mutex.lock();
            condition.wait(&mutex);
            mutex.unlock();
        }
        executeCycles--;

        // Ukonceni pri nastaveni priznaku ukonceni
        mutex.lock();
        if(abortFlag)
        {
            mutex.unlock();
            return;
        }
        mutex.unlock();

        // Provedeni prechodu automatu
        automaton->AutomatonStep(1, cellPossibilities, damageProbability);
        if(!resultChromosomeSetRepairment.empty())
            automatonRepairment->AutomatonStep(1, cellPossibilities, damageProbability);

        // Signal zasilajici aktualni stavy automatu
        Automaton::GridType gridRepairment;
        if(!resultChromosomeSetRepairment.empty())
            gridRepairment = automatonRepairment->grid;

        bool isLastGrid = (executeCycles == 0) ? true : false;   // Rozhodnuti, zda-li jde o posledni pozadovany prechod (bylo-li pozadovanych prechodu vice)
        emit GridGenerated(automaton->grid, gridRepairment, isLastGrid);
    }
}

void AnalyseManager::SetCycles(Automaton::GridType grid, Automaton::GridType gridRepairment, int cyclesCount, double damageProbability)
{
    // Probuzeni vlakna k provedeni dalsiho prechodu automatu
    mutex.lock();
    this->damageProbability = damageProbability;  // Nastaveni poskozeni
    executeCycles += cyclesCount;                 // Nastaveni poctu cyklu
    automaton->SetGrid(grid);
    if(!resultChromosomeSetRepairment.empty())
        automatonRepairment->SetGrid(gridRepairment);
    condition.wakeOne();
    mutex.unlock();
}

string AnalyseManager::PrintChromosome(Automaton::Chromosome &chromosome)
{
    stringstream out;
    string text = "";
    string instructionNeighbourhood;
    string instructionId;

    // Vypis instrukci v ramci chromozomu
    for(int i = 0; i < (int)chromosome.instrList.size(); i++)
    {
        stringstream out;
        out << (chromosome.instrList[i])->id;
        instructionId = out.str();
        string instructionNeighbourhood = ((chromosome.instrList[i])->neighbourhood == NEIGHBOURHOOD_VON_NEUMANN) ? "Von Neumann" : "Moore";

        text += "-  name: " + (chromosome.instrList[i])->name + ",\t id: " + instructionId + ",\t neighbour: " + instructionNeighbourhood + ",\t ";
        for(int j = 0; j < DIR_COUNT; j++)
        {
            stringstream out2;
            out2 << (int)(chromosome.instrList[i])->config[j];
            text += out2.str();
        }
        text += "\r\n";
    }

    return text;
}

string AnalyseManager::PrintChromosomeSet(Automaton::ChromosomeSet &chromosomeSet)
{
    stringstream out;
    string text = "";
    string instructionNeighbourhood;
    string instructionId;

    // Vypis instrukci v ramci chromozomu
    for(int i = 0; i < (int)chromosomeSet.size(); i++)
    {
        stringstream out;
        out << i;
        text += "- Bunka " + out.str() + ": ";

        for(int j = 0; j < (int)chromosomeSet[i].instrList.size(); j++)
        {
            stringstream out2;
            out2 << (int)(chromosomeSet[i].instrList[j])->id;
            text += out2.str() + " ";
        }
        text += "\r\n";
    }

    return text;
}
