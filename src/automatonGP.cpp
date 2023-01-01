#include "automatonGP.h"

using namespace std;

AutomatonGP::AutomatonGP(GridType &zygote, Chromosome &chromosome, E_NEIGHBOURHOOD neighbourhood)
{
    this->config = Config::GetInstance()->configInstance;
    this->grid = zygote;
    this->grid2 = zygote;
    this->grid3 = zygote;
    this->grid4 = zygote;
    this->cellCount = config->gridSize * config->gridSize;
    this->chromosome = chromosome;
    this->neighbourhood = neighbourhood;
}

void AutomatonGP::AutomatonStep(int stepCount)
{
    int cellNumber;       // cislo aktualni bunky

    // Docasna mrizka pro ulozeni zmen jednotlivych bunek (pro zajisteni paralelismu)
    GridType newGrid = grid;
    CellVector emptyCellVector;
    emptyCellVector.assign(config->gridSize, 0);
    //newGrid.assign(config->gridSize, emptyCellVector);

    for(int i = 0; i < stepCount; i++)
    {
        // Vymena mrizek
        this->grid4 = grid3;
        this->grid3 = grid2;
        this->grid2 = grid;

        // Docasna mrizka pro ulozeni priznaku aplikovani instrukce pro danou bunku (pote se jiz nezkousi aplikovat jina)
        GridType mapGrid;
        mapGrid.assign(config->gridSize, emptyCellVector);

        // Vypocitani nove aktualni mrizky
        for(int row = 0; row < config->gridSize; row++)
        {
            for(int col = 0; col < config->gridSize; col++)
            {
                cellNumber = row * config->gridSize + col;
                for(int j = 0; j < (int)chromosome.instrList.size(); j++)
                {
                    if((config->instructionApplication == true) && (mapGrid[row][col] == 1))
                    {   // Pro danou bunku jiz byla aktivovana instrukce (a je zapnuty rezim "aplikuj vsechny vhodne")
                        break;
                    }
                    else if((chromosome.instrList[j])->name == "NOP")
                    {   // NOP instrukce nejsou vyhodnocovany
                        continue;
                    }
                    SetNewConfiguration(mapGrid, cellNumber, newGrid, chromosome.instrList[j]);
                }
            }
        }
        this->grid = newGrid;
    }
}

void AutomatonGP::AutomatonStep(int stepCount, std::vector<int> &cellPossibilities, double damageProbability)
{
    // Provedeni prechodu
    AutomatonStep(stepCount);

    // Vliv "sumu" - nastaveni jine hodnoty pro bunku (s urcitou pravdepodobnosti)
    double randomProbability;
    int newIndex;

    for(int row = 0; row < config->gridSize; row++)
    {
        for(int col = 0; col < config->gridSize; col++)
        {
            randomProbability = ((double)rand() / RAND_MAX);
            if((randomProbability < damageProbability) && (grid[row][col] != 0) && !((config->task == TASK_SELF_ORGANIZATION) && (grid[row][col] == 2)))
            {
                do  // Vygenerovani jine hodnoty pro bunku
                {
                    newIndex = rand() % cellPossibilities.size();
                }
                while(cellPossibilities[newIndex] == grid[row][col]);

                grid[row][col] = cellPossibilities[newIndex];
            }
        }
    }
}

void AutomatonGP::SetNewConfiguration(GridType &mapGrid, int cellNumber, GridType &newGrid, Instruction *actInstruction)
{
    int row = (int)(cellNumber / (config->gridSize)); // nastaveni cisla radku odpovidajiciho cislu bunky
    int col = cellNumber % (config->gridSize);        // nastaveni cisla sloupce odpovidajiciho cislu bunky
    CellType actConfig[config->neighbourhood+1];      // pro ulozeni konfigurace aktualni bunky

    // Ziskani vstupni konfigurace aktualni bunky
    actConfig[DIR_CENTRAL] = newGrid[row][col];                 // Centralni bunka
    actConfig[DIR_TOP]     = grid[(row+(config->gridSize)-1)%(config->gridSize)][col];   // Horni soused
    actConfig[DIR_LEFT]    = grid[row][(col+(config->gridSize)-1)%(config->gridSize)];   // Levy soused
    actConfig[DIR_RIGHT]   = grid[row][(col+1)%(config->gridSize)];        // Pravy soused
    actConfig[DIR_BOTTOM]  = grid[(row+1)%(config->gridSize)][col];        // Dolni soused

    if(config->neighbourhood == (E_NEIGHBOURHOOD)NEIGHBOURHOOD_MOORE)
    {   // Pri pouziti Moorova okoli (8 sousedu + centralni bunka) musime vzit v potaz i sousedy na diagonalach
        actConfig[DIR_TOP_LEFT]     = grid[(row+(config->gridSize)-1)%(config->gridSize)][(col+(config->gridSize)-1)%(config->gridSize)];  // Levy horni soused
        actConfig[DIR_TOP_RIGHT]    = grid[(row+(config->gridSize)-1)%(config->gridSize)][(col+1)%(config->gridSize)];       // Pravy horni soused
        actConfig[DIR_BOTTOM_LEFT]  = grid[(row + 1) % (config->gridSize)][(col+(config->gridSize)-1)%(config->gridSize)];   // Levy dolni soused
        actConfig[DIR_BOTTOM_RIGHT] = grid[(row + 1) % (config->gridSize)][(col+1)%(config->gridSize)];                      // Pravy dolni soused
    }
    else
    {
        actConfig[DIR_TOP_LEFT]     = (CellType)0;  // Levy horni soused
        actConfig[DIR_TOP_RIGHT]    = (CellType)0;  // Pravy horni soused
        actConfig[DIR_BOTTOM_LEFT]  = (CellType)0;  // Levy dolni soused
        actConfig[DIR_BOTTOM_RIGHT] = (CellType)0;  // Pravy dolni soused
    }

    // Nastaveni stavu centralni bunky podle odpovidajici instrukce
    CellType newState;
    if((newState = GetNextState(actConfig, actInstruction)) != STATE_UNCHANGED)
    {
        newGrid[row][col] = newState;
        mapGrid[row][col] = 1;
    }
    else
        newGrid[row][col] = grid[row][col];
}

Automaton::CellType AutomatonGP::GetNextState(CellType actConfig[], Instruction *actInstruction)
{
    bool passCondition = true;

    // Pokus o aplikaci dane instrukce
    for(int i = 0; i < DIR_COUNT; i++)
    {
        if((actInstruction->config[i] > 0) && (actInstruction->config[i] != actConfig[i]))
        {   // Na stavu dane bunky zalezi (stav "0" znamena "nezalezi) a neshoduje se s predpisem instrukce -> instrukce se neaplikuje
            passCondition = false;
            break;
        }
    }

    if(passCondition)
        return actInstruction->output;

    return STATE_UNCHANGED;
}
