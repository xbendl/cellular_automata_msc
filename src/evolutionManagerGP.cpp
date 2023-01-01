#include "evolutionManagerGP.h"

using namespace std;

EvolutionManagerGP::EvolutionManagerGP(QObject *)
{
    // Nastaveni priznaku ukonceni a uspani
    abortFlag = false;
    sleepFlag = false;

    resultChromosome = NULL;
}

void EvolutionManagerGP::FindSolution(std::vector<Automaton::Instruction> instrList)
{
    this->instrList = instrList;
    this->instrList.insert(this->instrList.begin(), nopInstruction);
    QMutexLocker locker(&mutex);

    // Priprava pouzivanych vektoru ve vypoctu
    parentPop.clear();
    childPop.clear();
    tempPop.clear();
    tempMinPopulationFitness.clear();
    tempAvgPopulationFitness.clear();
    tempMaxPopulationFitness.clear();
    actGeneration  = 0;
    trainSet.clear();
    trainSetResult.clear();

    result.minPopulationFitness = result.minActPopulationFitness = result.minInitialPopulationFitness = FLT_MAX;
    result.evaluationCount = result.crossoverCount = result.mutationCount =
         result.avgPopulationFitness = result.avgActPopulationFitness = result.avgInitialPopulationFitness =
         result.maxPopulationFitness = result.maxActPopulationFitness = result.maxInitialPopulationFitness = 0;

    // Vygenerovani trenovaci mnoziny pocatecnich konfiguraci automatu
    GenerateTrainSet();
    cellCount = config->gridSize * config->gridSize;

    // Nastaveni priznaku ukonceni a uspani
    this->abortFlag = false;
    this->sleepFlag = false;

    // Spusteni evoluce
    if(!isRunning())
        start();
}

void EvolutionManagerGP::run()
{
    // Vytvoreni pocatecni populace jedincu
    GenerateInitPopulation();

    while(!Done())
    {
        // Provedeni jednoho evolucniho kroku
        Step();

        // Prubezne zapisovani statistika vyvoje fitness v populaci
        if(actGeneration % config->logInterval == 0)
            SetGenerationStatistics();

        // Zaslani informace o vygenerovanem chromozomu a aktualnim stavu vyvoje populace
        mutex.lock();
        if(!abortFlag)
        {
            float signalAvgFitness, signalMaxFitness;
            if(config->task == TASK_COMPUTATION_ADDER)
            {
                signalAvgFitness = ((float)result.avgInitialPopulationFitness) / (trainSet.size() * positionsAdder.size());
                signalMaxFitness = ((float)result.maxInitialPopulationFitness) / (trainSet.size() * positionsAdder.size());
            }
            else
            {
                signalAvgFitness = ((float)result.avgActPopulationFitness) / (trainSet.size() * cellCount);
                signalMaxFitness = ((float)result.maxActPopulationFitness) / (trainSet.size() * cellCount);
            }

            emit CurrentChromosomeGenerated(signalAvgFitness, signalMaxFitness);
        }
        mutex.unlock();

        // Uspani pri nastaveni priznaku uspani
        mutex.lock();
        if(sleepFlag)
        {
            condition.wait(&mutex);
            sleepFlag = false;
        }
        mutex.unlock();

        // Ukonceni pri nastaveni priznaku ukonceni
        mutex.lock();
        if(abortFlag)
        {
            // Vypocet a ulozeni doby behu
            struct timeval endTime;
            gettimeofday(&endTime, NULL);
            result.runningTime.tv_sec = endTime.tv_sec - startTime.tv_sec;
            if((endTime.tv_usec - startTime.tv_usec) < 0)
            {
                result.runningTime.tv_sec--;
                result.runningTime.tv_usec = (startTime.tv_usec - endTime.tv_usec) + 1;
            }
            else
            {
                result.runningTime.tv_usec = endTime.tv_usec - startTime.tv_usec;
            }
            mutex.unlock();
            return;
        }
        mutex.unlock();
    }

    // Zaslani informace o ukonceni evolucniho cyklu
    emit ChromosomeGenerated(true);
}

bool EvolutionManagerGP::Done()
{
    bool done = (actGeneration == config->generationCount) || abortFlag;

    // Akce po skonceni vypoctu
    if(done)
    {
        // Vypocet a ulozeni doby behu
        struct timeval endTime;
        gettimeofday(&endTime, NULL);
        result.runningTime.tv_sec = endTime.tv_sec - startTime.tv_sec;
        if((endTime.tv_usec - startTime.tv_usec) < 0)
        {
            result.runningTime.tv_sec--;
            result.runningTime.tv_usec = (startTime.tv_usec - endTime.tv_usec) + 1;
        }
        else
        {
            result.runningTime.tv_usec = endTime.tv_usec - startTime.tv_usec;
        }
    }

    return done;
}

void EvolutionManagerGP::Step()
{
    // Nastartovani casovace (pokud jde o prvni generaci)
    if(actGeneration == 0)
        gettimeofday(&startTime, NULL);

    int pIndex1, pIndex2;
    childPop.clear();
    tempPop.clear();

    // Vytvoreni mnoziny potomku pomoci operatoru krizeni
    while((int)childPop.size() < config->populationSize)
    {
        // Vyber rodicu
        pIndex1 = TournamentSelection(parentPop);
        pIndex2 = TournamentSelection(parentPop);

        // Tvorba novych potomku z vybranych potomku pomoci operatoru krizeni a mutace
        Crossover(parentPop[pIndex1], parentPop[pIndex2]);
    }

    // Ohodnoceni mnoziny potomku
    tempPop.resize(childPop.size() + parentPop.size());
    copy(parentPop.begin(), parentPop.end(), tempPop.begin());
    copy(childPop.begin(), childPop.end(), (tempPop.begin() + parentPop.size()));
    parentPop.clear();
    EvaluatePopulation(tempPop);

    // Aplikace elitismu - nejlepsi potomci jsou automaticky zkopirovani do nove populace
    parentPop.resize(config->elitismSize);
    sort(tempPop.begin(), tempPop.end(), FitnessSortGP);
    copy(tempPop.begin(), (tempPop.begin() + config->elitismSize), parentPop.begin());
    tempPop.erase(tempPop.begin(), (tempPop.begin() + config->elitismSize));

    // Provedeni mutace na jedincich nepodlehajicich elitismu
    for(int i = config->elitismSize; i < (int)tempPop.size(); i++)
        Mutation(tempPop[i]);

    resultChromosome = &(parentPop[0]);

    // Obnova populace
    Survive(parentPop, tempPop);
    this->actGeneration++;
}

void EvolutionManagerGP::SetGenerationStatistics()
{
    result.minActPopulationFitness = INT_MAX;
    result.maxActPopulationFitness = 0;
    result.avgActPopulationFitness = 0;

    for(int i = 0; i < (int)parentPop.size(); i++)
    {
        if(parentPop[i].fitness > result.maxActPopulationFitness)
            result.maxActPopulationFitness = parentPop[i].fitness;
        if(parentPop[i].fitness > result.maxPopulationFitness)
            result.maxPopulationFitness = parentPop[i].fitness;
        if(parentPop[i].fitness < result.minActPopulationFitness)
            result.minActPopulationFitness = parentPop[i].fitness;
        if(parentPop[i].fitness < result.minPopulationFitness)
            result.minPopulationFitness = parentPop[i].fitness;
        result.avgActPopulationFitness += parentPop[i].fitness;
    }
    result.avgActPopulationFitness = result.avgActPopulationFitness / parentPop.size();

    tempAvgPopulationFitness.push_back(result.avgActPopulationFitness);
    tempMinPopulationFitness.push_back(result.minActPopulationFitness);
    tempMaxPopulationFitness.push_back(result.maxActPopulationFitness);
}

void EvolutionManagerGP::GenerateInitPopulation()
{
    Automaton::Chromosome chromosome;
    chromosome.fitness = 0;

    bool found;
    int randIndex = 0;

    // Vytvoreni nahodnych chromozomu
    for(int i = 0; i < config->populationSize; i++)
    {
        chromosome.instrList.clear();

        // Vytvoreni posloupnosti instrukci pro jeden chromozom
        for(int j = 0; j < config->instructionCount; j++)
        {
            if(((double)rand() / INT_MAX) < config->nopProbability)
            {   // Vlozeni NOP instrukce
                chromosome.instrList.push_back(&(instrList[0]));
            }
            else
            {
                found = true;
                // Nalezeni instrukce do chromozomu, ktera se tam jeste nenachazi
                while(found)
                {
                    randIndex = rand() % (instrList.size() - 1) + 1;
                    found = false;
                    for(int k = 0; k < (int)chromosome.instrList.size(); k++)
                    {
                        if((chromosome.instrList[k])->id == instrList[randIndex].id)
                        {
                            found = true;
                            break;
                        }
                    }
                }
                chromosome.instrList.push_back(&(instrList[randIndex]));
            }
        }
        parentPop.push_back(chromosome);
    }

    // Ohodnoceni pocatecni populace jedincu
    EvaluatePopulation(parentPop);

    // Ulozeni statistik pocatecni populace jedincu
    result.minInitialPopulationFitness = FLT_MAX;
    result.maxInitialPopulationFitness = 0;
    result.avgInitialPopulationFitness = 0;
    for(int i = 0; i < (int)parentPop.size(); i++)
    {
        if(parentPop[i].fitness > result.maxInitialPopulationFitness)
            result.maxInitialPopulationFitness = parentPop[i].fitness;
        if(parentPop[i].fitness > result.maxPopulationFitness)
            result.maxPopulationFitness = parentPop[i].fitness;
        if(parentPop[i].fitness < result.minInitialPopulationFitness)
            result.minInitialPopulationFitness = parentPop[i].fitness;
        if(parentPop[i].fitness < result.minPopulationFitness)
            result.minPopulationFitness = parentPop[i].fitness;
        result.avgInitialPopulationFitness += parentPop[i].fitness;
    }
    result.avgInitialPopulationFitness = result.avgInitialPopulationFitness / parentPop.size();
    tempAvgPopulationFitness.push_back(result.avgInitialPopulationFitness);
    tempMinPopulationFitness.push_back(result.minInitialPopulationFitness);
    tempMaxPopulationFitness.push_back(result.maxInitialPopulationFitness);

    // Zaslani informace o vygenerovanem chromozomu a aktualnim stavu vyvoje populace
    mutex.lock();
    if(!abortFlag)
    {
        float signalAvgFitness, signalMaxFitness;
        if(config->task == TASK_COMPUTATION_ADDER)
        {
            signalAvgFitness = ((float)result.avgInitialPopulationFitness) / (trainSet.size() * positionsAdder.size());
            signalMaxFitness = ((float)result.maxInitialPopulationFitness) / (trainSet.size() * positionsAdder.size());
        }
        else
        {
            signalAvgFitness = ((float)result.avgInitialPopulationFitness) / (trainSet.size() * cellCount);
            signalMaxFitness = ((float)result.maxInitialPopulationFitness) / (trainSet.size() * cellCount);
        }
        emit CurrentChromosomeGenerated(signalAvgFitness, signalMaxFitness);
    }
    mutex.unlock();
}

void EvolutionManagerGP::Mutation(Automaton::Chromosome &chromosome)
{
    double randValue;

    // Mutace vybrane instrukce
    randValue = ((double) rand() / RAND_MAX);
    if(randValue < config->mutationProbability)
    {
        RecombinationOperator::RandomMutation(instrList, chromosome);
        result.mutationCount++;
    }
}

void EvolutionManagerGP::Crossover(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2)
{
    RecombinationOperator::RandomCrossover(parent1, parent2, childPop);
    result.crossoverCount++;
}

void EvolutionManagerGP::StopEvoluteInstructions()
{
    if(sleepFlag)
    {   // Obnoveni prehravani
        mutex.lock();
        sleepFlag = false;
        condition.wakeOne();
        mutex.unlock();
    }
    else
    {   // Pozastaveni prehravani
        mutex.lock();
        sleepFlag = true;
        mutex.unlock();
    }
}

void EvolutionManagerGP::AbortEvoluteInstructions()
{
    sleepFlag = false;
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();
    wait();                // Pockani na dobehnuti vlakna
}

void EvolutionManagerGP::Survive(vector<Automaton::Chromosome> &population, vector<Automaton::Chromosome> &selectedPopulation)
{
    int championIndex;

    // Vybrani nejschopnejsich jedincu do dalsi populace
    while((int)population.size() < config->populationSize)
    {
        championIndex = TournamentSelection(selectedPopulation);
        population.push_back(selectedPopulation[championIndex]);
        selectedPopulation.erase(selectedPopulation.begin() + championIndex);
    }
}

int EvolutionManagerGP::TournamentSelection(vector<Automaton::Chromosome> &population)
{
    // Nahodny vyber jednotlivcu z populace (ucastniku "turnaje")
    int randIndex, maxIndex = 0;
    float maxFitness = 0;

    for(int i = 0; i < config->tournamentSize; i++)
    {
        randIndex = rand() % population.size();
        if(population[randIndex].fitness > maxFitness)
        {
            maxFitness = population[randIndex].fitness;
            maxIndex = randIndex;
        }
    }

    return maxIndex;
}

void EvolutionManagerGP::EvaluatePopulation(std::vector<Automaton::Chromosome> &population)
{
    for(int i = 0; i < (int)population.size(); i++)
    {   // ohodnoti se vsechny jedinci populace
        EvaluateChromosome(population[i]);
    }
}

void EvolutionManagerGP::EvaluateChromosome(Automaton::Chromosome &chromosome)
{
    chromosome.fitness = 0;

    // Vypocet fitness na zaklade otestovani vyvoje automatu na vsech trenovacich mnozinach.
    for(int i = 0; i < (int)trainSet.size(); i++)
    {
        Automaton *automatonGP = new AutomatonGP(trainSet[i], chromosome, (E_NEIGHBOURHOOD)config->neighbourhood);
        automatonGP->AutomatonStep(config->automatonStep);  // Provedeni pozadovaneho poctu kroku automatu
        PrintChromosome(chromosome);

        if(config->task == TASK_MAJORITY)
        {
            if(this->trainSetResult[i] == 1)
                chromosome.fitness += Automaton::HammingDistanceGP(automatonGP->grid, whiteGrid);
            else
                chromosome.fitness += Automaton::HammingDistanceGP(automatonGP->grid, blackGrid);
        }
        else if(config->task == TASK_SYNCHRONIZATION)
        {
            /*if(this->trainSetResult[i] == 1)
            {
                float fitness = 0;
                fitness += Automaton::HammingDistanceGP(automatonGP->grid,  blackGrid);
                fitness += Automaton::HammingDistanceGP(automatonGP->grid2, whiteGrid);
                fitness += Automaton::HammingDistanceGP(automatonGP->grid3, blackGrid);
                fitness += Automaton::HammingDistanceGP(automatonGP->grid4, whiteGrid);
                fitness =  ((float) fitness) / 4;
                chromosome.fitness += fitness;
            }
            if(this->trainSetResult[i] == 2)
            {
                float fitness = 0;
                fitness += Automaton::HammingDistanceGP(automatonGP->grid,  whiteGrid);
                fitness += Automaton::HammingDistanceGP(automatonGP->grid2, blackGrid);
                fitness += Automaton::HammingDistanceGP(automatonGP->grid3, whiteGrid);
                fitness += Automaton::HammingDistanceGP(automatonGP->grid4, blackGrid);
                fitness =  ((float) fitness) / 4;
                chromosome.fitness += fitness;
            }*/
            float fitness1 = 0;
            fitness1 += Automaton::HammingDistanceGP(automatonGP->grid,  blackGrid);
            fitness1 += Automaton::HammingDistanceGP(automatonGP->grid2, whiteGrid);
            fitness1 += Automaton::HammingDistanceGP(automatonGP->grid3, blackGrid);
            fitness1 += Automaton::HammingDistanceGP(automatonGP->grid4, whiteGrid);
            fitness1 =  ((float) fitness1) / 4;
            float fitness2 = 0;
            fitness2 += Automaton::HammingDistanceGP(automatonGP->grid,  whiteGrid);
            fitness2 += Automaton::HammingDistanceGP(automatonGP->grid2, blackGrid);
            fitness2 += Automaton::HammingDistanceGP(automatonGP->grid3, whiteGrid);
            fitness2 += Automaton::HammingDistanceGP(automatonGP->grid4, blackGrid);
            fitness2 =  ((float) fitness2) / 4;
            chromosome.fitness += (fitness1 > fitness2) ? fitness1 : fitness2;
        }
        else if(config->task == TASK_COMPUTATION_ADDER)
        {
            float fitness = 0;
            fitness += Automaton::HammingDistanceComputationTask(automatonGP->grid, patternSet[i], positionsAdder);
            chromosome.fitness += fitness;
        }
        else if(config->task == TASK_SELF_ORGANIZATION)
        {
            float actFitness;
            actFitness = Automaton::HammingDistanceGP(automatonGP->grid,  pattern);

            // Provedeni dalsiho kroku a aktualizace fitness (pro overeni, ze konfigurace zustava ve finalnim stavu)
            automatonGP->AutomatonStep(1);
            actFitness += Automaton::HammingDistanceGP(automatonGP->grid,  pattern);
            chromosome.fitness = actFitness / 2;
        }

        delete automatonGP;
    }
    result.evaluationCount++;
}

EvolutionManagerGP::~EvolutionManagerGP()
{
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();

    wait();                // Pockani na dobehnuti vlakna
}

bool EvolutionManagerGP::SaveChromosome(std::string outputFile)
{
    ofstream file;
    file.open(outputFile.c_str());

    // Vypocet prumerne ceny za vsechny generace
    float multConstant = (float)1 / ((float)actGeneration / config->logInterval + 1);
    for(int i = 0; i < (int)tempAvgPopulationFitness.size(); i++)
        result.avgPopulationFitness += tempAvgPopulationFitness[i] * multConstant;

    file << "*** INSTRUKCEMI RIZENY CELULARNI AUTOMAT ***\r\n\r\n";
    string taskName;
    if(config->task == TASK_MAJORITY)
        taskName = "MAJORITY";
    else if(config->task == TASK_SYNCHRONIZATION)
        taskName = "SYNCHRONIZATION";
    else if(config->task == TASK_SELF_ORGANIZATION)
        taskName = "SELF_ORGANIZATION";
    else if(config->task == TASK_COMPUTATION_ADDER)
        taskName = "COMPUTATION_ADDER";

    string neighbourhoodName = (config->neighbourhood == NEIGHBOURHOOD_VON_NEUMANN) ? "VON_NEUMANN" : "MOORE";
    string cellularProgrammingName = (config->cellularProgramming) ? "TRUE" : "FALSE";
    file << "Uloha: \t" + taskName + "\r\n";
    file << "Okoli: \t" + neighbourhoodName + "\r\n";
    file << "Celularni programovani: \t" + cellularProgrammingName + "\r\n";
    file << "Instrukce: \t";
    for(int i = 0; i < (int)(resultChromosome)->instrList.size(); i++)
    {
        stringstream ss;
        ss << ((resultChromosome)->instrList[i])->id;
        file << ss.str() << " ";
    }
    file << "\r\n\r\n";

    file << "*** PARAMETRY BEHU GA ***\r\n";
    file << actGeneration                << "\t # dokoncena generace behu vypoctu\r\n";
    file << config->instructionCount     << "\t # pocet instrukci v ramci chromozomu\r\n";
    file << result.crossoverCount        << "\t # pocet aplikaci operatoru krizeni od inicializace\r\n";
    file << result.mutationCount         << "\t # pocet aplikaci operatoru mutace od inicializace\r\n";
    file << result.evaluationCount       << "\t # pocet ohodnoceni genomu od inicializace\r\n";
    file << config->trainSetSize         << "\t # pocet vzorku v trenovaci mnozine\r\n";
    file << config->gridSize             << "\t # rozmer mrizky vzorku z trenovaci mnoziny\r\n\r\n";

    file << "*** CELKOVE STATISTIKY ***\r\n";
    file << result.runningTime.tv_sec << "." << result.runningTime.tv_usec << "s\t # doba behu vypoctu\r\n";
    file << result.minPopulationFitness      << "\t # nejhorsi fitness od inicializace\r\n";
    file << result.maxPopulationFitness      << "\t # nejlepsi fitness od inicializace\r\n";
    file << result.avgPopulationFitness      << "\t # prumerne fitness od inicializace\r\n\r\n";

    file << "*** STATISTIKY K POCATECNI POPULACI ***\r\n";
    file << result.minInitialPopulationFitness   << "\t # nejhorsi fitness v pocatecni populaci\r\n";
    file << result.maxInitialPopulationFitness   << "\t # nejlepsi fitness v pocatecni populaci\r\n";
    file << result.avgInitialPopulationFitness   << "\t # prumerna fitness v pocatecni populaci\r\n\r\n";

    file << "*** STATISTIKY K AKTUALNI POPULACI ***\r\n";
    file << result.minActPopulationFitness   << "\t # nejhorsi fitness v aktualni populaci\r\n";
    file << result.maxActPopulationFitness   << "\t # nejlepsi fitness v aktualni populaci\r\n";
    file << result.avgActPopulationFitness   << "\t # prumerna fitness v aktualni populaci\r\n\r\n";

    file << "*** MEZIVYSLEDKY ***\r\n";
    for(int i = 0; i < (int)tempMinPopulationFitness.size(); i++)
    {
        file << (i * config->logInterval) << " \t";
        if(config->task == TASK_COMPUTATION_ADDER)
        {   // Uloha mechanickeho vypoctu vypocitava celkove fitness jinak nez ostatni ulohy
            file << setprecision(3) << ((double)tempMinPopulationFitness[i] / (trainSet.size() * positionsAdder.size())) << " \t";
            file << ((double)tempAvgPopulationFitness[i] / (trainSet.size() * positionsAdder.size())) << " \t";
            file << ((double)tempMaxPopulationFitness[i] / (trainSet.size() * positionsAdder.size())) << "\r\n";
        }
        else
        {
            file << setprecision(3) << ((double)tempMinPopulationFitness[i] / (cellCount * config->trainSetSize)) << " \t";
            file << ((double)tempAvgPopulationFitness[i] / (cellCount * config->trainSetSize)) << " \t";
            file << ((double)tempMaxPopulationFitness[i] / (cellCount * config->trainSetSize)) << "\r\n";
        }
    }

    file.close();

    return true;
}

bool FitnessSortGP(Automaton::Chromosome inv1, Automaton::Chromosome inv2)
{
    return (inv1.fitness > inv2.fitness);
}
