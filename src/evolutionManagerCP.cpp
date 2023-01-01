#include "evolutionManagerCP.h"

using namespace std;

EvolutionManagerCP::EvolutionManagerCP(QObject *)
{
    // Nastaveni priznaku ukonceni a uspani
    abortFlag = false;
    sleepFlag = false;
}

void EvolutionManagerCP::FindSolution(std::vector<Automaton::Instruction> instrList)
{
    // Priprava pouzivanych promennych v evolucnim cyklu
    actGeneration = 0;
    trainSet.clear();
    trainSetResult.clear();
    tempAvgFitness.clear();
    tempAvgFitnessRepairment.clear();
    startTime.tv_sec = 0;
    startTime.tv_usec = 0;

    result.evaluationCount = result.crossoverCount = result.mutationCount = result.avgFitness = 0;

    QMutexLocker locker(&mutex);

    // Vygenerovani trenovaci mnoziny pocatecnich konfiguraci automatu
    GenerateTrainSet();

    // Priprava pouzivanych promennych v evolucnim cyklu (II.)
    this->instrList = instrList;
    this->instrList.insert(this->instrList.begin(), nopInstruction);
    cellCount = config->gridSize * config->gridSize;
    initialStep = config->initialPhaseStep;

    prevFitness.assign(cellCount, 0);
    maxFitness.assign(cellCount, 0);
    fitnessSum.assign(cellCount, 0);
    bestChromosomeSetFitness = 0;
    bestChromosomeSetFitnessRepairment = 0;
    bestChromosomeSet.clear();
    bestChromosomeSetRepairment.clear();

    // Nastaveni priznaku ukonceni a uspani
    this->abortFlag = false;
    this->sleepFlag = false;

    // Spusteni evoluce
    if(!isRunning())
        start();
}

void EvolutionManagerCP::run()
{
    // Vytvoreni pocatecni populace jedincu
    GenerateInitPopulation();

    // Evolucni cyklus
    selfRepairmentLoop = false;
    while(!Done(tempAvgFitness))
    {
        // Provedeni jednoho evolucniho kroku
        if(config->task == TASK_COMPUTATION_ADDER)
            StepComputation(chromosomeSet, bestChromosomeSet, bestChromosomeSetFitness, tempAvgFitness);
        else
            Step(chromosomeSet, bestChromosomeSet, bestChromosomeSetFitness, tempAvgFitness);

        // Zaslani informace o vygenerovanem chromozomu a aktualnim stavu vyvoje populace
        mutex.lock();
        if(!abortFlag)
            emit CurrentChromosomeGenerated(tempAvgFitness.back(), bestChromosomeSetFitness);
        mutex.unlock();

        // Uspani pri nastaveni priznaku uspani
        mutex.lock();
        if(sleepFlag)
        {
            condition.wait(&mutex);
            sleepFlag = false;
        }
        mutex.unlock();
    }

    // Zaslani informace o ukonceni evolucniho cyklu
    if((config->task != TASK_SELF_ORGANIZATION) || abortFlag)
    {   // Pokud se nejedna o ulohu sebe-organizace (v nemz je potreba zkoumat sebe-opravu), je zaroven ukoncen cely evolucni beh
        emit ChromosomeGenerated(false);
        return;
    }
    else
        emit ChromosomeGenerated(true);

    // Hledani pravidel pro opravu poskozeneho vzoru (v pripade ulohy typu self-organization)
    actGeneration = 0;
    initialStep = config->initialPhaseStep;
    prevFitness.clear();
    maxFitness.clear();
    fitnessSum.clear();
    prevFitness.assign(cellCount, 0);
    maxFitness.assign(cellCount, 0);
    fitnessSum.assign(cellCount, 0);

    // Nastaveni chromozomu pro pocatecni populaci
    chromosomeSetRepairment = chromosomeSet;

    // Naplneni trenovaci mnoziny vygenerovanymi poskozenymi vzory
    GenerateDamagedPopulation();

    // Evolucni cyklus (trenink sebe-opravy)
    selfRepairmentLoop = true;
    while(!Done(tempAvgFitnessRepairment))
    {
        // Provedeni jednoho evolucniho kroku
        Step(chromosomeSetRepairment, bestChromosomeSetRepairment, bestChromosomeSetFitnessRepairment, tempAvgFitnessRepairment);

        // Zaslani informace o vygenerovanem chromozomu a aktualnim stavu vyvoje populace
        mutex.lock();
        if(!abortFlag)
            emit CurrentChromosomeGenerated(tempAvgFitnessRepairment.back(), tempAvgFitnessRepairment.back());

        mutex.unlock();

        // Uspani pri nastaveni priznaku uspani
        mutex.lock();
        if(sleepFlag)
        {
            condition.wait(&mutex);
            sleepFlag = false;
        }
        mutex.unlock();
    }

    // Zaslani informace o ukonceni evolucniho cyklu
    emit ChromosomeGenerated(false);
}

bool EvolutionManagerCP::Done(std::vector<double> &tempAvgFitness)
{
    bool done = (actGeneration == config->generationCount) || (tempAvgFitness[tempAvgFitness.size()-1] == 1) || abortFlag;

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

void EvolutionManagerCP::StepComputation(Automaton::ChromosomeSet &chromosomeSet, Automaton::ChromosomeSet &bestChromosomeSet, double &bestChromosomeSetFitness, std::vector<double> &tempAvgFitness)
{
    // Nastartovani casovace (pokud jde o prvni generaci)
    if((actGeneration == 0) && (startTime.tv_sec == 0))
        gettimeofday(&startTime, NULL);

    // Aplikace rekombinacnich operatoru
    Automaton::ChromosomeSet newChromosomes;
    Automaton::Chromosome selectedChromosome, selectedChromosome2;
    Automaton::ChromosomeSet newChromosomeSet = chromosomeSet;

    int randNumber = rand() % 3 + 1;  // pocet mutovanych bunek
    int position;
    vector<Automaton::Point> mutatedCells;
    for(int i = 0; i < randNumber; i++)
    {
        selectedChromosome = newChromosomeSet[rand() % newChromosomeSet.size()];
        if(((double)rand() / RAND_MAX) < 0.5)
        {   // Mutace posloupnosti instrukci nahodne zvolene bunky
            RecombinationOperator::RandomMutation(instrList, newChromosomeSet[randNumber]);
        }
        else
        {   // Sestaveni nove posloupnosti instrukci pro bunku s vyuzitim operatoru krizeni
            position = rand() % newChromosomeSet.size();    // Prvni vybrana posloupnost instrukci
            selectedChromosome = chromosomeSet[position];
            position = rand() % newChromosomeSet.size();    // Druha vybrana posloupnost instrukci
            selectedChromosome2 = chromosomeSet[position];
            RecombinationOperator::RandomCrossover(selectedChromosome, selectedChromosome2, newChromosomes);  // Krizeni vybranych pravidel
            newChromosomeSet[randNumber] = newChromosomes[0];
            newChromosomes.clear();
        }
    }

    // Ohodnoceni aktualniho reseni
    double actAvgFitness = EvaluateSolution(newChromosomeSet);

    // Nastaveni pomocneho vektoru udrzujiciho fitness bunek predchoziho kroku
    for(int i = 0; i < cellCount; i++)
        prevFitness[i] = chromosomeSet[i].fitness;

    // Ulozeni ohodnoceni aktualniho reseni
    if((this->actGeneration % config->logInterval) == 0)
        tempAvgFitness.push_back(actAvgFitness);

    // Test, zda je aktualni reseni lepsi nez nejlepsi dosud nalezene (nebo alespon stejne dobre)
    if(actAvgFitness >= bestChromosomeSetFitness)
    {
        chromosomeSet = newChromosomeSet;
        bestChromosomeSet = newChromosomeSet;
        bestChromosomeSetFitness = actAvgFitness;
    }

    this->actGeneration++;
}

void EvolutionManagerCP::Step(Automaton::ChromosomeSet &chromosomeSet, Automaton::ChromosomeSet &bestChromosomeSet, double &bestChromosomeSetFitness, std::vector<double> &tempAvgFitness)
{
    // Nastartovani casovace (pokud jde o prvni generaci)
    if((actGeneration == 0) && (startTime.tv_sec == 0))
        gettimeofday(&startTime, NULL);

    // Nastavuje pro kazdou bunku po kazdem vyvojovem kroku pocet po sobe jdoucich kroku, kdy mela bunka maximalni fitness
    if(config->lockStepCount != -1)
        CheckConfigurations(chromosomeSet, config->trainSetSize);

    // V pripade velmi nizke uspesnosti instrukci jsou instrukce vsech bunek mrizky restartovany
    if((config->resetGridPercentageLimit != -1) && (IsRecommendResetGrid(chromosomeSet, actGeneration, config->trainSetSize)))
    {
        prevFitness.assign(cellCount, 0);
        maxFitness.assign(cellCount, 0);
        fitnessSum.assign(cellCount, 0);
        initialStep = actGeneration + config->initialPhaseStep;
        GenerateChromosomeSet();  // opetovne generovani instrukci pro vsechny bunky automatu
    }

    // V pripade dlouhodobe neuspesnosti posloupnosti instrukci bunky je tato posloupnost instrukci resetovana
    if(config->resetCellPercentageLimit != -1)
        SolveLongtermUnsuccess(chromosomeSet, actGeneration, config->trainSetSize);

    // Aplikace rekombinacnich operatoru
    int fitterNeighbourCount, position;
    vector<int> fitterNeighbourPosition;
    Automaton::ChromosomeSet newChromosomes;
    Automaton::Chromosome selectedChromosome, selectedChromosome2;
    Automaton::ChromosomeSet newChromosomeSet = chromosomeSet;

    for(int i = 0; i < cellCount; i++)
    {
        fitterNeighbourCount = GetFitterNeighbours(i, fitterNeighbourPosition);  // Zjisteni poctu lepe ohodnocenych sousedu
        if((this->actGeneration > 1) && (fitterNeighbourCount == 0) && (chromosomeSet[i].fitness == prevFitness[i]) && (chromosomeSet[i].fitness != config->trainSetSize))
        {   // Nemeni-li se v dvou po sobe jdoucich krocich fitness bunky a zaroven tato bunka nedosahuje maximalniho hodnoty fitness, je provedena mutace instrukcni sady teto bunky
            if(((double)rand() / RAND_MAX) < config->mutationProbability)
            {
                RecombinationOperator::RandomMutation(instrList, newChromosomeSet[i]);
                result.mutationCount++;
            }
        }
        else if((this->actGeneration > 1) && (fitterNeighbourCount > 0) && (config->respectFitnessImprovement) && (chromosomeSet[i].fitness > prevFitness[i]))
        {   // Pouze mutace soucasne sady instrukci (lze sledovat pozitivni vyvoj hodnoty fitness)
            if(((double)rand() / RAND_MAX) < config->mutationProbability)
            {
                RecombinationOperator::RandomMutation(instrList, newChromosomeSet[i]);
                result.mutationCount++;
            }
        }
        else if(fitterNeighbourCount == 1)
        {   // Nahrazeni aktualni posloupnosti instrukci mutovanou variantou posloupnosti instrukci lepe ohodnoceneho souseda
            selectedChromosome = newChromosomeSet[fitterNeighbourPosition.front()];
            if(((double)rand() / RAND_MAX) < config->mutationProbability)
            {
                RecombinationOperator::RandomMutation(instrList, selectedChromosome);
                result.mutationCount++;
            }
            newChromosomeSet[i] = selectedChromosome;
        }
        else if(fitterNeighbourCount > 1)
        {   // Nahrazeni aktualni posloupnosti instrukci novou mnozinou vzniklou krizenim dvou nahodne vybranych, lepe ohodnocenych, instrukcnich mnozin a nasledne provedenim mutace takto vznikle instrukcni mnoziny
            position = rand() % fitterNeighbourPosition.size();    // Prvni vybrana posloupnost instrukci
            selectedChromosome = chromosomeSet[fitterNeighbourPosition[position]];
            fitterNeighbourPosition.erase(fitterNeighbourPosition.begin() + position);

            position = rand() % fitterNeighbourPosition.size();    // Druha vybrana posloupnost instrukci
            selectedChromosome2 = chromosomeSet[fitterNeighbourPosition[position]];
            fitterNeighbourPosition.erase(fitterNeighbourPosition.begin() + position);

            RecombinationOperator::RandomCrossover(selectedChromosome, selectedChromosome2, newChromosomes);  // Krizeni vybranych pravidel
            newChromosomeSet[i] = newChromosomes[0];
            newChromosomes.clear();

            result.crossoverCount++;
        }
        fitterNeighbourPosition.clear();
    }
    chromosomeSet = newChromosomeSet;

    // Ohodnoceni aktualniho reseni
    double actAvgFitness = EvaluateSolution(chromosomeSet);

    // Nastaveni pomocneho vektoru udrzujiciho fitness bunek predchoziho kroku
    for(int i = 0; i < cellCount; i++)
        prevFitness[i] = chromosomeSet[i].fitness;

    // Ulozeni ohodnoceni aktualniho reseni
    if((this->actGeneration % config->logInterval) == 0)
        tempAvgFitness.push_back(actAvgFitness);

    // Kontrola uspesnosti instrukci vygenerovanych v tomto kroku
    if(actAvgFitness > bestChromosomeSetFitness)
    {
        bestChromosomeSet = chromosomeSet;
        bestChromosomeSetFitness = actAvgFitness;
    }

    this->actGeneration++;
}

int EvolutionManagerCP::GetFitterNeighbours(int cellNumber, std::vector<int> &fitterNeighbourPosition)
{
    int actualCellFitness = chromosomeSet[cellNumber].fitness;    // fitness aktualni bunky
    int row = (int)(cellNumber / config->gridSize); // nastaveni cisla radku odpovidajiciho cislu bunky
    int col = cellNumber % config->gridSize;        // nastaveni cisla sloupce odpovidajiciho cislu bunky
    vector<int> position;

    position.push_back((((row + config->gridSize - 1) % config->gridSize) * config->gridSize) + col);                       // horni soused
    position.push_back((row * config->gridSize) + ((col + config->gridSize - 1) % config->gridSize));                       // levy soused
    position.push_back((row * config->gridSize) + ((col + 1) % config->gridSize));                              // pravy soused
    position.push_back((((row + 1) % config->gridSize) * config->gridSize) + col);                              // dolni soused
    if(config->extendedInstructionCalculation || config->neighbourhood == 1)
    {   // Nova instrukcni sada se bude vypocitavat z 9 okoli jen pokud je to povoleno v konfiguracnim souboru ci je-li nastavene 8-okoli
        position.push_back((((row + config->gridSize - 1) % config->gridSize) * config->gridSize) + ((col+config->gridSize-1)%config->gridSize));   // levy horni soused
        position.push_back((((row + config->gridSize - 1) % config->gridSize) * config->gridSize) + ((col+1)%config->gridSize));        // pravy horni soused
        position.push_back((((row + 1) % config->gridSize) * config->gridSize) + ((col+config->gridSize-1)%config->gridSize));          // levy dolni soused
        position.push_back((((row + 1) % config->gridSize) * config->gridSize) + ((col+1)%config->gridSize));               // pravy dolni soused
    }
    SortByFitness(position);   // Srovnani pozic ukazatelu na bunky podle jejich fitness

    for(unsigned int i = 0; i < position.size(); i++)
    {   // Pro kazdeho souseda
        if((chromosomeSet[position[i]].fitness > actualCellFitness) && ((config->lockStepCount == -1) || (maxFitness[position[i]] < config->lockStepCount)))
        {   // Ma-li vybrany soused vetsi fitness nez aktualni bunka a dany soused nema zamknute instrukce, ulozime isntrukce jeho souseda do prislusneho vektoru
            fitterNeighbourPosition.push_back(position[i]);
        }
    }

    return fitterNeighbourPosition.size();
}

void EvolutionManagerCP::GenerateInitPopulation()
{
    // Vygenerovani pocatecni posloupnosti instrukci
    GenerateChromosomeSet();

    // Ohodnoceni pocatecni populace jedincu
    double avgActFitness = EvaluateSolution(chromosomeSet);
    tempAvgFitness.push_back(avgActFitness);

    // Zaslani informace o vygenerovanem chromozomu a aktualnim stavu vyvoje populace
    mutex.lock();
    if(!abortFlag)
        emit CurrentChromosomeGenerated(tempAvgFitness.back(), tempAvgFitness.back());

    mutex.unlock();
}

void EvolutionManagerCP::GenerateDamagedPopulation()
{
    // Vygenerovani pocatecni posloupnosti instrukci
    trainSet.clear();
    GenerateDamagedSet(trainSet);
    config->trainSetSize = trainSet.size();

    // Ohodnoceni pocatecni populace jedincu
    double avgActFitness = EvaluateSolution(chromosomeSet);
    tempAvgFitnessRepairment.push_back(avgActFitness);

    // Zaslani informace o vygenerovanem chromozomu a aktualnim stavu vyvoje populace
    mutex.lock();
    if(!abortFlag)
        emit CurrentChromosomeGenerated(tempAvgFitnessRepairment.back(), tempAvgFitnessRepairment.back());
    mutex.unlock();
}

void EvolutionManagerCP::CheckConfigurations(Automaton::ChromosomeSet &chromosomeSet, int configurationCount)
{
    for(int i = 0; i < cellCount; i++)
    {
        if(chromosomeSet[i].fitness != configurationCount)
        {   // Nektery z automatu populace ma bunku v necilovem stavu
            maxFitness[i] = 0;
        }
        else
        {
            maxFitness[i]++;
        }
    }
}

bool EvolutionManagerCP::IsRecommendResetGrid(Automaton::ChromosomeSet &chromosomeSet, int evolutionStep, int configurationCount)
{
    int correctCellValue = 0;

    for(int i = 0; i < cellCount; i++)
    {   // Soucet fitness vsech bunek mrizky
        correctCellValue += chromosomeSet[i].fitness;
    }

    if((((double)correctCellValue / (cellCount * configurationCount)) < config->resetGridPercentageLimit) && (evolutionStep > initialStep))
    {   // Reset instrukcnich mnozin vsech bunek mrizky
        return true;
    }

    return false;
}

void EvolutionManagerCP::SolveLongtermUnsuccess(Automaton::ChromosomeSet &chromosomeSet, int evolutionStep, int configurationCount)
{
    // Vygenerovani nove posloupnosti instrukci pro neuspesnou bunku
    if(evolutionStep > initialStep)
    {   // Reset bunky prichazi v uvahu az po uvodni fazi
        for(int i = 0; i < cellCount; i++)
        {
            fitnessSum[i] += chromosomeSet[i].fitness;
            if(((evolutionStep % config->fitnessMeasuringSequence) == 0) && (evolutionStep != 0))
            {   // Pokud je dosazeno pozadovaneho poctu cyklu
                if(fitnessSum[i] < floor(config->resetCellPercentageLimit * config->fitnessMeasuringSequence * configurationCount))
                {   // Pokud byla instrukcni mnozina neuspesna, vygeneruje se na jeji misto nova nahodna mnozina.
                    GenerateChromosome(i);
                }
                fitnessSum[i] = 0;
            }
        }
    }
}

void EvolutionManagerCP::GenerateChromosome(int cellNumber)
{
    Automaton::Chromosome chromosome;
    chromosome.fitness = 0;

    bool found;
    int randIndex = 0;

    // Vytvoreni posloupnosti instrukci pro jeden chromozom
    for(int i = 0; i < config->instructionCount; i++)
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

    chromosomeSet[cellNumber] = chromosome;
}

void EvolutionManagerCP::GenerateChromosomeSet()
{
    chromosomeSet.clear();

    for(int cellNumber = 0; cellNumber < cellCount; cellNumber++)
    {
        Automaton::Chromosome chromosome;
        chromosome.fitness = 0;

        bool found;
        int randIndex = 0;

        // Vytvoreni posloupnosti instrukci pro jeden chromozom
        for(int i = 0; i < config->instructionCount; i++)
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
        chromosomeSet.push_back(chromosome);
    }
}

double EvolutionManagerCP::EvaluateSolution(Automaton::ChromosomeSet &chromosomeSet)
{
    int overallFitness = 0;
    double actAverageFitness;

    // Vynulovani fitness bunek
    for(int cellNumber = 0; cellNumber < cellCount; cellNumber++)
        chromosomeSet[cellNumber].fitness = 0;

    // Zjisteni fitness
    for(int i = 0; i < (int)trainSet.size(); i++)
    {   // ohodnoti se vsechny konfigurace z trenovaci mnoziny
        AutomatonCP *automaton = new AutomatonCP(trainSet[i], chromosomeSet, (E_NEIGHBOURHOOD)config->neighbourhood);
        automaton->AutomatonStep(config->automatonStep);  // Provedeni pozadovaneho poctu kroku automatu

        if(config->task == TASK_MAJORITY)
        {   // Vypocet fitness pro ulohu majority
            if(this->trainSetResult[i] == 1)
                overallFitness += Automaton::HammingDistanceCP(automaton->grid, whiteGrid, chromosomeSet);
            else
                overallFitness += Automaton::HammingDistanceCP(automaton->grid, blackGrid, chromosomeSet);
        }
        else if(config->task == TASK_SYNCHRONIZATION)
        {   // Vypocet fitness pro ulohu synchronizace
            double actFitnes = 0;

            if(this->trainSetResult[i] == 1)
            {
                actFitnes += Automaton::HammingDistanceCP(automaton->grid,  blackGrid, chromosomeSet);
                actFitnes += Automaton::HammingDistanceCP(automaton->grid2, whiteGrid, chromosomeSet);
                actFitnes += Automaton::HammingDistanceCP(automaton->grid3, blackGrid, chromosomeSet);
                actFitnes += Automaton::HammingDistanceCP(automaton->grid4, whiteGrid, chromosomeSet);
            }
            if(this->trainSetResult[i] == 2)
            {
                actFitnes += Automaton::HammingDistanceCP(automaton->grid,  whiteGrid, chromosomeSet);
                actFitnes += Automaton::HammingDistanceCP(automaton->grid2, blackGrid, chromosomeSet);
                actFitnes += Automaton::HammingDistanceCP(automaton->grid3, whiteGrid, chromosomeSet);
                actFitnes += Automaton::HammingDistanceCP(automaton->grid4, blackGrid, chromosomeSet);
            }
            overallFitness += floor((double)actFitnes / 4);
        }
        else if(config->task == TASK_COMPUTATION_ADDER)
        {   // Vypocet fitness pro ulohu mechanickeho vypoctu
            overallFitness += Automaton::HammingDistanceComputationTask(automaton->grid, patternSet[i], positionsAdder);
        }
        else if(config->task == TASK_SELF_ORGANIZATION)
        {   // Vypocet fitness pro ulohu sebe-organizace
            vector<float> tempFitness;
            for(int i = 0; i < (int)chromosomeSet.size(); i++)
                tempFitness.push_back(chromosomeSet[i].fitness);

            int actFitness = Automaton::HammingDistanceCP(automaton->grid,  pattern, chromosomeSet);

            // Provedeni dalsiho kroku a aktualizace fitness (pro overeni, ze konfigurace zustava ve finalnim stavu)
            automaton->AutomatonStep(1);
            actFitness += Automaton::HammingDistanceCP(automaton->grid,  pattern, chromosomeSet);

            for(int i = 0; i < (int)chromosomeSet.size(); i++)
                chromosomeSet[i].fitness = tempFitness[i] + (chromosomeSet[i].fitness - tempFitness[i]) / 2;

            overallFitness += actFitness / 2;
        }
        delete automaton;
    }

    // Normalizace fitness do intervalu <0;1>
    if(config->task == TASK_SELF_ORGANIZATION)
    {
        if(!selfRepairmentLoop)
            actAverageFitness = (double)overallFitness / cellCount;
        else
            actAverageFitness = (double)overallFitness / (cellCount * config->damagedConfigurationCount);
    }
    else if(config->task == TASK_COMPUTATION_ADDER)
        actAverageFitness = (double)overallFitness / (positionsAdder.size() * trainSet.size());
    else
        actAverageFitness = (double)overallFitness / (cellCount * config->trainSetSize);

    result.evaluationCount++;

    return actAverageFitness;
}

EvolutionManagerCP::~EvolutionManagerCP()
{
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();

    wait();                // Pockani na dobehnuti vlakna
}

bool EvolutionManagerCP::SaveChromosome(std::string outputFile)
{
    ofstream file;
    file.open(outputFile.c_str());

    // Vypocet prumerne ceny za vsechny generace
    float multConstant = (float)1 / ((float)actGeneration / config->logInterval + 1);
    for(int i = 0; i < (int)tempAvgFitness.size(); i++)
        result.avgFitness += tempAvgFitness[i] * multConstant;

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
    file << "Instrukce: \r\n";
    for(int i = 0; i < (int)bestChromosomeSet.size(); i++)
    {
        for(int j = 0; j < (int)bestChromosomeSet[i].instrList.size(); j++)
        {
            stringstream ss;
            ss << (bestChromosomeSet[i].instrList[j])->id;
            file << ss.str() << " ";
        }
        file << "\r\n";
    }
    file << "\r\n";

    if((config->task == TASK_SELF_ORGANIZATION) && config->cellularProgramming)
    {
        file << "Instrukce (sebe-oprava): \r\n";
        for(int i = 0; i < (int)bestChromosomeSetRepairment.size(); i++)
        {
            for(int j = 0; j < (int)bestChromosomeSetRepairment[i].instrList.size(); j++)
            {
                stringstream ss;
                ss << (bestChromosomeSet[i].instrList[j])->id;
                file << ss.str() << " ";
            }
            file << "\r\n";
        }
        file << "\r\n";
    }

    file << "*** PARAMETRY BEHU GA ***\r\n";
    file << actGeneration                << "\t # dokoncena generace behu vypoctu\r\n";
    file << config->instructionCount     << "\t # pocet instrukci v ramci chromozomu\r\n";
    file << result.crossoverCount        << "\t # pocet aplikaci operatoru krizeni od inicializace\r\n";
    file << result.mutationCount         << "\t # pocet aplikaci operatoru mutace od inicializace\r\n";
    file << result.evaluationCount       << "\t # pocet ohodnoceni genomu od inicializace\r\n";
    file << config->trainSetSize         << "\t # pocet vzorku v trenovaci mnozine\r\n";
    file << config->gridSize             << "\t # rozmer mrizky vzorku z trenovaci mnoziny\r\n";
    if(config->task == TASK_SELF_ORGANIZATION)
        file << config->trainSetSize              << "\t # pocet vzorku v trenovaci mnozine\r\n\r\n";
    else
        file << config->damagedConfigurationCount << "\t # pocet poskozenych vzoru pouzivanych pro trenink sebeopravy\r\n\r\n";

    file << "*** CELKOVE STATISTIKY ***\r\n";
    file << result.runningTime.tv_sec << "." << result.runningTime.tv_usec << "s\t # doba behu vypoctu\r\n";
    file << result.avgFitness         << "\t # prumerne fitness od inicializace\r\n";
    file << bestChromosomeSetFitness  << "\t # nejlepsi fitness od inicializace\r\n\r\n";

    file << "*** MEZIVYSLEDKY ***\r\n";
    for(int i = 0; i < (int)tempAvgFitness.size(); i++)
    {
        file << setprecision(3) << (i * config->logInterval) << " \t";
        file << tempAvgFitness[i] << " \r\n";
    }
    file << "\r\n";

    if(tempAvgFitnessRepairment.size() > 0)
    {
        file << "*** MEZIVYSLEDKY (SEBE-OPRAVA) ***\r\n";
        for(int i = 0; i < (int)tempAvgFitnessRepairment.size(); i++)
        {
            file << setprecision(3) << (i * config->logInterval) << " \t";
            file << tempAvgFitnessRepairment[i] << " \r\n";
        }
    }

    file.close();

    return true;
}

void EvolutionManagerCP::StopEvoluteInstructions()
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

void EvolutionManagerCP::AbortEvoluteInstructions()
{
    sleepFlag = false;
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();
    wait();                // Pockani na dobehnuti vlakna
}

void EvolutionManagerCP::SortByFitness(std::vector<int> &position)
{
    vector<int> helpVector;
    int maxFitness, selectedPos;

    while(!position.empty())
    {
        maxFitness = 0;
        selectedPos = 0;

        for(unsigned int i = 0; i < position.size(); i++)
        {
            if(chromosomeSet[position[i]].fitness > maxFitness)
            {
                maxFitness = chromosomeSet[position[i]].fitness;
                selectedPos = i;
            }
        }
        helpVector.push_back(position[selectedPos]);
        position.erase(position.begin() + selectedPos);
    }

    position = helpVector;
}
