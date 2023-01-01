#include "evolutionManager.h"

using namespace std;

EvolutionManager::EvolutionManager()
{
    // Inicializace instrukce NOP (no-operation)
    nopInstruction.id = 0;
    nopInstruction.output = Automaton::STATE_UNCHANGED;
    nopInstruction.name = "NOP";
    nopInstruction.neighbourhood = NEIGHBOURHOOD_VON_NEUMANN;
    for(int i = 0; i < DIR_COUNT; i++)
        nopInstruction.config[i] = 0;

    config = Config::GetInstance()->configInstance;
}

bool EvolutionManager::LoadSelfOrganizationTask(std::string inputFile, std::vector<int> &cellPossibilities)
{
    zygote.clear();
    pattern.clear();

    // Otevreni souboru
    std::string path = QApplication::applicationDirPath().toStdString() + "/" + inputFile;
    ifstream inFile;
    inFile.open(inputFile.c_str(), std::ifstream::in);
    if(inFile.fail())
    {   // Vstupni soubor se nepodarilo otevrit
        return false;
    }

    // Ziskani oznaceni souboru
    string line;
    string fileContent = "*** INSTRUKCEMI RIZENY CELULARNI AUTOMAT - ULOHA ***";
    getline(inFile, line);
    if(line.substr(0, fileContent.size()) != fileContent)
    {   // Soubor neobsahuje korektni oznaceni
        return false;
    }
    getline(inFile, line);  // preskoceni prazneho radku

    // Ziskani rozmeru mrizky (udaj na prvnim radku souboru)
    int size = 0;
    if(getline(inFile, line))
        size = atoi(line.c_str());

    // Ziskani moznych stavu bunky
    getline(inFile, line);
    int possibility;
    while(line.find_first_of(';') != string::npos)
    {
        possibility = atoi((line.substr(0, line.find_first_of(';'))).c_str());
        cellPossibilities.push_back(possibility);
        line.erase(0, line.find_first_of(';') + 1);
    }

    // Ziskani zygoty
    Automaton::CellVector helpVector;  // pro docasne ulozeni jednoho radku
    int helpInt;
    for(int i = 0; i < size; i++)
    {   // Postupne nacitani radku
        getline(inFile, line);
        helpVector.clear();
        for(int j = 0; j < size; j++)
        {
            helpInt = atoi(line.substr(j, 1).c_str());
            helpVector.push_back(helpInt);
        }
        zygote.push_back(helpVector);
    }
    getline(inFile, line);  // preskoceni prazdneho radku oddelujiciho zygotu od vzoru

    // Ziskani hledaneho vzoru
    for(int i = 0; i < size; i++)
    {   // Postupne nacitani radku
        getline(inFile, line);
        helpVector.clear();
        for(int j = 0; j < size; j++)
        {
            helpInt = atoi(line.substr(j, 1).c_str());
            helpVector.push_back(helpInt);
        }
        pattern.push_back(helpVector);
    }

    config->trainSetSize = 1;           // trenovani probiha pouze na zygote
    config->gridSize = pattern.size();  // nastaveni rozmeru mrizky podle rozmeru hledaneho vzoru
    cellCount = config->gridSize * config->gridSize;

    inFile.close();
    return true;
}

void EvolutionManager::GenerateTrainSet()
{
    Automaton::GridType trainGrid;
    Automaton::CellVector cellVector, cellVector2;

    cellPossibilities.clear();
    trainSet.clear();
    trainSetResult.clear();
    positionsAdder.clear();
    patternSet.clear();

    if((config->task == TASK_MAJORITY) || (config->task == TASK_SYNCHRONIZATION))
    {
        cellPossibilities.push_back(1);
        cellPossibilities.push_back(2);

        float tresh;
        float randValue;
        Automaton::CellType cellValue;
        int zeroCount;
        for(int x = 0; x < config->trainSetSize; x++)
        {
            trainGrid.clear();
            tresh = (float)rand() / INT_MAX;
            zeroCount = 0;
            for(int i = 0; i < config->gridSize; i++)
            {
                cellVector.clear();
                for(int j = 0; j < config->gridSize; j++)
                {
                    randValue = (float)rand() / INT_MAX;
                    cellValue = (randValue > tresh) ? 2 : 1;
                    zeroCount += (cellValue == 1) ? 1 : 0;
                    cellVector.push_back(cellValue);
                }
                trainGrid.push_back(cellVector);
            }
            trainSet.push_back(trainGrid);
            if(zeroCount > ((config->gridSize * config->gridSize) / 2))
                trainSetResult.push_back(2);
            else
                trainSetResult.push_back(1);
        }
        cellVector.assign(config->gridSize, (Automaton::CellType)1);
        cellVector2.assign(config->gridSize, (Automaton::CellType)2);
        blackGrid.assign(config->gridSize, cellVector);
        whiteGrid.assign(config->gridSize, cellVector2);
    }
    else if(config->task == TASK_COMPUTATION_ADDER)
    {
        trainSet.push_back(GenerateTrainGridAdder(1, 1, 1, 1, 1));
        trainSet.push_back(GenerateTrainGridAdder(1, 1, 3, 1, 1));
        trainSet.push_back(GenerateTrainGridAdder(1, 3, 1, 1, 1));
        trainSet.push_back(GenerateTrainGridAdder(1, 3, 3, 1, 1));
        trainSet.push_back(GenerateTrainGridAdder(3, 1, 1, 1, 1));
        trainSet.push_back(GenerateTrainGridAdder(3, 1, 3, 1, 1));
        trainSet.push_back(GenerateTrainGridAdder(3, 3, 1, 1, 1));
        trainSet.push_back(GenerateTrainGridAdder(3, 3, 3, 1, 1));

        patternSet.push_back(GenerateTrainGridAdder(1, 1, 1, 1, 1));
        patternSet.push_back(GenerateTrainGridAdder(1, 1, 3, 1, 3));
        patternSet.push_back(GenerateTrainGridAdder(1, 3, 1, 1, 3));
        patternSet.push_back(GenerateTrainGridAdder(1, 3, 3, 3, 1));
        patternSet.push_back(GenerateTrainGridAdder(3, 1, 1, 1, 3));
        patternSet.push_back(GenerateTrainGridAdder(3, 1, 3, 3, 1));
        patternSet.push_back(GenerateTrainGridAdder(3, 3, 1, 3, 1));
        patternSet.push_back(GenerateTrainGridAdder(3, 3, 3, 3, 3));

        Automaton::Point point;
        point.x = 2;
        point.y = 0;
        positionsAdder.push_back(point);
        point.x = 2;
        point.y = 1;
        positionsAdder.push_back(point);

        config->gridSize = 3;
    }
    else if(config->task == TASK_SELF_ORGANIZATION)
    {
        LoadSelfOrganizationTask(config->taskFile, cellPossibilities);
        config->trainSetSize = 1;   // trenovani probiha pouze na zygote (bez ohledu na nastaveni v formulari)
        trainSet.push_back(zygote);
    }
}

Automaton::GridType EvolutionManager::GenerateTrainGridAdder(Automaton::CellType var_A, Automaton::CellType var_B, Automaton::CellType var_Ci, Automaton::CellType var_C0, Automaton::CellType var_S)
{
    Automaton::CellVector cellVector;
    cellVector.assign(3, (Automaton::CellType)1);

    Automaton::GridType trainGrid;
    trainGrid.assign(3, cellVector);

    Automaton::GridType trainGrid1;
    trainGrid[0][0] = var_A;
    trainGrid[0][1] = var_B;
    trainGrid[0][2] = var_Ci;
    trainGrid[2][0] = var_C0;
    trainGrid[2][1] = var_S;

    return trainGrid;
}

void EvolutionManager::GenerateDamagedSet(std::vector<Automaton::GridType> &damagedSet)
{
    damagedSet.clear();
    Automaton::GridType damagedGrid;   // Pomocna pro ukladani matice reprezentujici poskozeny vzor
    Automaton::CellVector rowValues;   // Pomocna pro ukladani hodnot na radku matice reprezentujici poskozeny vzor
    double randomTreshold;             // Procentualni poskozeni mrizky
    double randomProbability;          // Pomocna pro generovani nahodnych hodnot
    int newIndex;

    for(int gridIndex = 0; gridIndex < config->damagedConfigurationCount; gridIndex++)  // Vektor mrizek
    {
        damagedGrid.clear();
        randomTreshold  = (double)rand() / RAND_MAX;
        randomTreshold *= config->maxDamagedPercentage;

        for(int row = 0; row < config->gridSize; row++)      // Jedna mrizka
        {
            rowValues.clear();
            for(int col = 0; col < config->gridSize; col++)  // Jeden radek mrizky
            {
                randomProbability = ((double)rand() / RAND_MAX);
                if((pattern[row][col] != 0) && (randomProbability < randomTreshold))
                {
                    do  // Vygenerovani jine hodnoty pro bunku
                    {
                        newIndex = rand() % cellPossibilities.size();
                    }
                    while((cellPossibilities[newIndex] == 0) || (cellPossibilities[newIndex] == pattern[row][col]));

                    rowValues.push_back(cellPossibilities[newIndex]);
                }
                else
                {
                    rowValues.push_back(pattern[row][col]);
                }
            }
            damagedGrid.push_back(rowValues);
        }
        damagedSet.push_back(damagedGrid);
    }
}

string EvolutionManager::PrintChromosome(Automaton::Chromosome &chromosome)
{
    stringstream ss;
    ss << chromosome.fitness;

    string text = "Chromosome: \n- fitness:\t" + ss.str() + "\n- instructions:\t";

    // Vypis instrukci v ramci chromozomu
    for(int i = 0; i < (int)chromosome.instrList.size(); i++)
    {
        stringstream ss;
        ss << (chromosome.instrList[i])->id;
        text += ss.str() + " ";
    }
    text += "\n";

    return text;
}
