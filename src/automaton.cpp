#include "automaton.h"

using namespace std;

int Automaton::HammingDistanceGP(GridType grid1, GridType grid2)
{
    // Kontrola stejne velikosti automatu
    if(grid1.size() != grid2.size())
        return -1;

    // Porovnavani jednotlivych bunek
    int hammingDistance = 0;
    for(unsigned int row = 0; row < grid1.size(); row++)
    {
        for(unsigned int col = 0; col < grid1[0].size(); col++)
        {
            if(grid1[row][col] == grid2[row][col])
            {
                hammingDistance++;
            }
        }
    }

    return hammingDistance;
}

int Automaton::HammingDistanceComputationTask(GridType grid1, GridType grid2, std::vector<Point> positionsAdder)
{
    // Kontrola stejne velikosti automatu
    if(grid1.size() != grid2.size())
        return -1;

    int hammingDistance = 0;
    for(int i = 0; i < (int)positionsAdder.size(); i++)
    {
        if(grid1[positionsAdder[i].x][positionsAdder[i].y] == grid2[positionsAdder[i].x][positionsAdder[i].y])
        {
            hammingDistance++;
        }
    }

    return hammingDistance;
}

int Automaton::HammingDistanceCP(GridType grid1, GridType grid2, Automaton::ChromosomeSet &chromosomeSet)
{
    int cellNumber;

    // Kontrola stejne velikosti automatu
    if(grid1.size() != grid2.size())
        return -1;

    // Porovnavani jednotlivych bunek
    int hammingDistance = 0;
    for(unsigned int row = 0; row < grid1.size(); row++)
    {
        for(unsigned int col = 0; col < grid1[0].size(); col++)
        {
            if(grid1[row][col] == grid2[row][col])
            {
                hammingDistance++;
                cellNumber = row * grid1.size() + col;
                chromosomeSet[cellNumber].fitness++;
            }
        }
    }

    return hammingDistance;
}

void Automaton::PrintGrid()
{
    for(int i = 0; i < (config->gridSize); i++)
        cout << "__";
    cout << "__" << endl;

    for(int row = 0; row < (config->gridSize); row++)
    {
        cout << "|";
        for(int col = 0; col < (config->gridSize); col++)
        {
            cout << (int)grid[row][col] << " ";
        }
        cout << "|";
        cout << endl;
    }

    for(int i = 0; i < (config->gridSize); i++)
        cout << "__";
    cout << "__" << endl;
}

void Automaton::SetGrid(Automaton::GridType grid)
{
    this->grid = grid;
}
