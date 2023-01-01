#include "recombinationOperator.h"

using namespace std;

void RecombinationOperator::RandomCrossover(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::ChromosomeSet &childPop)
{
    int crossoverType = rand() % CROSSOVER_COUNT;   // Nahodna volba rekombinacniho operatoru krizeni
    Automaton::Chromosome child1, child2;
    child1.fitness = 0;
    child2.fitness = 0;

    switch(crossoverType)
    {
        case CROSSOVER_PMX:
            CrossoverPMX(parent1, parent2, child1);
            childPop.push_back(child1);
            break;
        case CROSSOVER_OX:
            CrossoverOX(parent1, parent2, child1);
            childPop.push_back(child1);
            break;
        case CROSSOVER_AP:
            CrossoverAP(parent1, parent2, child1);
            CrossoverAP(parent2, parent1, child2);
            childPop.push_back(child1);
            childPop.push_back(child2);
            break;
        case CROSSOVER_MPX:
            CrossoverMPX(parent1, parent2, child1);
            childPop.push_back(child1);
            break;
    }
}

void RecombinationOperator::CrossoverPMX(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::Chromosome &child)
{
    int instructionCount = (Config::GetInstance())->instructionCount;

    // Ziskani indexu pro body krizeni
    int crossPoint1 = rand() % instructionCount;
    int crossPoint2 = rand() % instructionCount;

    int tempPoint;
    if(crossPoint1 > crossPoint2)
    {
        tempPoint = crossPoint1;
        crossPoint1 = crossPoint2;
        crossPoint2 = tempPoint;
    }

    // Vymena podretezcu urcena body krizeni
    child.instrList.resize(instructionCount, NULL);
    for(int i = crossPoint1; i <= crossPoint2; i++)
    {
        child.instrList[i] = parent1.instrList[i];      // zamena prostredni casti mezi krizicimi body
    }

    // Aktualni gen je spojen s protilehlym genem a s genem se stejnou hodnotou v pomocnem chromozomu
    Automaton::Instruction *temp;
    Automaton::Chromosome parent = parent2;
    vector<int> dummyVector;
    for(int i = crossPoint1; i <= crossPoint2; i++)
    {
        for(int j = 0; j < instructionCount; j++)
        {
            if(parent.instrList[j] == child.instrList[i])
            {
                temp = parent.instrList[i];
                parent.instrList[i] = parent.instrList[j];
                parent.instrList[j]= temp;
                dummyVector.push_back(i);
                dummyVector.push_back(j);
                break;
            }
        }
    }

    // Na potomka jsou zkopirovany pouze ty geny z pomocneho chromozomu, ktere jsou mimo body krizeni
    for(int i = 0; i < (int)dummyVector.size(); i++)
        if((dummyVector[i] < crossPoint1) || (dummyVector[i] > crossPoint2))
            child.instrList[dummyVector[i]] = parent.instrList[dummyVector[i]];

    // Zbyla mista doplnime podle templatu - druheho rodicovskeho chromozomu
    for(int i = crossPoint2 + 1; i < instructionCount; i++)
        if(child.instrList[i] == NULL)
            child.instrList[i] = parent2.instrList[i];
    for(int i = 0; i < crossPoint1; i++)
        if(child.instrList[i] == NULL)
            child.instrList[i] = parent2.instrList[i];
}

void RecombinationOperator::CrossoverOX(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::Chromosome &child)
{
    int instructionCount = (Config::GetInstance())->instructionCount;
    vector<int> usedInstructions;

    // Ziskani indexu pro body krizeni
    int crossPoint1 = rand() % instructionCount;
    int crossPoint2 = rand() % instructionCount;
    int tempPoint;
    if(crossPoint1 > crossPoint2)
    {
        tempPoint = crossPoint1;
        crossPoint1 = crossPoint2;
        crossPoint2 = tempPoint;
    }

    // Kopirovani useku vymezeneho krizicimi body geny z genomu rodice 1
    child.instrList.resize(instructionCount, NULL);
    for(int i = crossPoint1; i <= crossPoint2; i++)
    {
        child.instrList[i] = parent1.instrList[i];
        usedInstructions.push_back((child.instrList[i])->id);
    }

    // Doplneni useku mimo krizici body geny z genomu rodice 2 (zacina se za druhym krizicim bodem)
    vector<int>::iterator it;
    int indexChild  = (crossPoint2 + 1) % instructionCount;
    int indexParent = (crossPoint2 + 1) % instructionCount;
    while((int)usedInstructions.size() != instructionCount)
    {   // dokud neni vyplneny cely genom potomka
        if(((parent2.instrList[indexParent]->name == "NOP")) || ((it = find(usedInstructions.begin(), usedInstructions.end(), (parent2.instrList[indexParent])->id)) == usedInstructions.end()))
        {   // instrukce zatim neni u potomka pouzita - pouzijeme ji
            child.instrList[indexChild] = parent2.instrList[indexParent];
            usedInstructions.push_back((parent2.instrList[indexParent])->id);
            indexChild = (indexChild + 1) % instructionCount;
        }

        indexParent = (indexParent + 1) % instructionCount;
    }
}

void RecombinationOperator::CrossoverAP(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::Chromosome &child)
{
    int instructionCount = (Config::GetInstance())->instructionCount;

    // Stridave pokusy o pridani instukce do potomka ze stran obou rodicu
    vector<Automaton::Instruction *>::iterator it;
    for(int i = 0; i < instructionCount; i++)
    {
        if((parent1.instrList[i]->name == "NOP") || (((it = find(child.instrList.begin(), child.instrList.end(), (parent1.instrList[i]))) == child.instrList.end())))
            child.instrList.push_back(parent1.instrList[i]);
        if(instructionCount == (int)child.instrList.size())
            break;

        if((parent2.instrList[i]->name == "NOP") || (((it = find(child.instrList.begin(), child.instrList.end(), (parent2.instrList[i]))) == child.instrList.end())))
            child.instrList.push_back(parent2.instrList[i]);
        if(instructionCount == (int)child.instrList.size())
            break;
    }
}

void RecombinationOperator::CrossoverMPX(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::Chromosome &child)
{
    int instructionCount = (Config::GetInstance())->instructionCount;

    // Urceni delky vybirane podcesty
    int randCount = rand() % instructionCount;
    int startIndex = rand() % (instructionCount - randCount);

    // Ziskani instrukci vybrane podcesty
    vector<Automaton::Instruction *> dummyVector(randCount);
    copy((parent1.instrList.begin() + startIndex), (parent1.instrList.begin() + startIndex + randCount), dummyVector.begin());

    // Kopirovani vybrane podcesty na zacatek chromozomu potomka
    child.instrList.resize(randCount);
    copy(dummyVector.begin(), dummyVector.end(), child.instrList.begin());

    // Doplneni potomka o cesty z druheho rodice (brano pruchodem zleva doprava)
    vector<Automaton::Instruction *>::iterator it;
    for(int i = 0; i < (int)parent2.instrList.size(); i++)
        if((parent2.instrList[i]->name == "NOP") || (((it = find(dummyVector.begin(), dummyVector.end(), parent2.instrList[i])) == dummyVector.end()) && ((int)child.instrList.size() < instructionCount)))
            child.instrList.push_back(parent2.instrList[i]);
}

void RecombinationOperator::RandomMutation(std::vector<Automaton::Instruction> &instrList, Automaton::Chromosome &chromosome)
{
    int mutationType = rand() % MUTATION_COUNT;   // Nahodna volba rekombinacniho operatoru mutace
    switch(mutationType)
    {
        case MUTATION_SWAP:
            MutationSwap(chromosome);
            break;
        case MUTATION_IVM:
            MutationIvm(chromosome);
            break;
        case MUTATION_DM:
            MutationDm(chromosome);
            break;
        case MUTATION_SM:
            MutationSm(chromosome);
            break;
        case MUTATION_ISM:
            MutationIsm(instrList, chromosome);
            break;
    }
}

void RecombinationOperator::MutationSwap(Automaton::Chromosome &chromosome)
{
    int instructionCount = (Config::GetInstance())->instructionCount;

    // Nahodne vygenerovani indexu dvou instrukci, ve kterych dojde k zamene
    int randInstruction1 = rand() % instructionCount;
    int randInstruction2 = rand() % instructionCount;
    Automaton::Instruction *tempInstruction;

    // Zamena vybranych instrukci
    tempInstruction = chromosome.instrList[randInstruction1];
    chromosome.instrList[randInstruction1] = chromosome.instrList[randInstruction2];
    chromosome.instrList[randInstruction2] = tempInstruction;
}

void RecombinationOperator::MutationIvm(Automaton::Chromosome &chromosome)
{
    int instructionCount = (Config::GetInstance())->instructionCount;

    // Nahodne vygenerovani indexu dvou instrukci, mezi kterymi bude provedena inverze
    int randInstruction1 = rand() % instructionCount;
    int randInstruction2 = rand() % instructionCount;

    int tempInstruction;
    if(randInstruction1 > randInstruction2)
    {
        tempInstruction = randInstruction1;
        randInstruction1 = randInstruction2;
        randInstruction2 = tempInstruction;
    }

    // Inverze posloupnosti instrukci mezi dvema vybranymi body
    vector<Automaton::Instruction *> dummyVector;
    dummyVector.resize(randInstruction2 - randInstruction1 + 1);
    copy(&(chromosome.instrList[randInstruction1]), &(chromosome.instrList[randInstruction2])+1, dummyVector.begin());
    reverse(dummyVector.begin(), dummyVector.end());
    chromosome.instrList.erase((chromosome.instrList.begin() + randInstruction1), (chromosome.instrList.begin() + randInstruction2 + 1));

    // Vlozeni inverzni posloupnosti na nahodne misto posloupnosti
    int randInsertPosition = 0;
    if(chromosome.instrList.size() > 0)
        randInsertPosition = rand() % chromosome.instrList.size();
    chromosome.instrList.insert((chromosome.instrList.begin() + randInsertPosition), dummyVector.begin(), dummyVector.end());
}

void RecombinationOperator::MutationDm(Automaton::Chromosome &chromosome)
{
    int instructionCount = (Config::GetInstance())->instructionCount;

    // Nahodne vygenerovani indexu dvou instrukci, mezi kterymi bude provedena inverze
    int randInstruction1 = rand() % instructionCount;
    int randInstruction2 = rand() % instructionCount;
    int tempInstruction;
    if(randInstruction1 > randInstruction2)
    {
        tempInstruction = randInstruction1;
        randInstruction1 = randInstruction2;
        randInstruction2 = tempInstruction;
    }

    // Ziskani posloupnosti instrukci mezi dvema vybranymi body
    vector<Automaton::Instruction *> dummyVector;
    dummyVector.resize(randInstruction2 - randInstruction1 + 1);
    copy(&(chromosome.instrList[randInstruction1]), &(chromosome.instrList[randInstruction2])+1, dummyVector.begin());
    chromosome.instrList.erase((chromosome.instrList.begin() + randInstruction1), (chromosome.instrList.begin() + randInstruction2 + 1));

    // Vlozeni inverzni posloupnosti na nahodne misto cesty
    int randInsertPosition = 0;
    if(chromosome.instrList.size() > 0)
        randInsertPosition = rand() % chromosome.instrList.size();
    chromosome.instrList.insert((chromosome.instrList.begin() + randInsertPosition), dummyVector.begin(), dummyVector.end());
}

void RecombinationOperator::MutationSm(Automaton::Chromosome &chromosome)
{
    int instructionCount = (Config::GetInstance())->instructionCount;

    // Nahodne vygenerovani indexu dvou instrukci, mezi kterymi budou instrukce nahodne prohazene
    int randInstruction1 = rand() % instructionCount;
    int randInstruction2 = rand() % instructionCount;
    int tempInstruction;
    if(randInstruction1 > randInstruction2)
    {
        tempInstruction = randInstruction1;
        randInstruction1 = randInstruction2;
        randInstruction2 = tempInstruction;
    }

    // Ziskani posloupnosti instrukci mezi dvema vybranymi body
    vector<Automaton::Instruction *> dummyVector;
    dummyVector.resize(randInstruction2 - randInstruction1 + 1);
    copy(&(chromosome.instrList[randInstruction1]), &(chromosome.instrList[randInstruction2])+1, dummyVector.begin());

    int randIndex;
    for(int i = randInstruction1; i <= randInstruction2; i++)
    {
        randIndex = rand() % dummyVector.size();
        chromosome.instrList[i] = dummyVector[randIndex];
        dummyVector.erase(dummyVector.begin() + randIndex);
    }
}

void RecombinationOperator::MutationIsm(std::vector<Automaton::Instruction> &instrList, Automaton::Chromosome &chromosome)
{
    // Ziskani nahodne vybrane instrukce
    Automaton::Instruction *randInstruction = ChooseRandomInstruction(instrList, chromosome);

    // Smazani nahodne vybrane vybrane instrukce
    int randIndex = rand() % chromosome.instrList.size();
    chromosome.instrList.erase(chromosome.instrList.begin() + randIndex);

    // Vlozeni vybrane instrukce na jinou nahodnou pozici
    int randInsertIndex = rand() % chromosome.instrList.size();
    chromosome.instrList.insert(chromosome.instrList.begin() + randInsertIndex, randInstruction);
}

Automaton::Instruction* RecombinationOperator::ChooseRandomInstruction(std::vector<Automaton::Instruction> &instrList, Automaton::Chromosome &chromosome)
{
    bool found = true;
    int randIndex;

    // Nalezeni instrukce do chromozomu, ktera se tam jeste nenachazi
    while(found)
    {
        found = false;
        randIndex = rand() % (instrList.size() - 1) + 1;
        vector<Automaton::Instruction>::iterator it;
        for(int i = 0; i < (int)chromosome.instrList.size(); i++)
        {
            if((chromosome.instrList[i] != NULL) && (chromosome.instrList[i])->id == instrList[randIndex].id)
            {
                found = true;
                break;
            }
        }
    }

    return &(instrList[randIndex]);
}
