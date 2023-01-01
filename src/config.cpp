#include "config.h"

using namespace std;

Config::Config()
{
    populationSize = 100;
    generationCount = 1000;
    neighbourhood = 0;
    tournamentSize = 2;
    automatonStep = 32;
    logInterval = 1;
    mutationProbability = 0;
    defaultStepInterval = 1;
    evolutionGraphValuesCount = 100;
    timeIntervalEvolutionInstructions = 250;
    timeIntervalAnalyseInstructions = 250;
    elitismSize = 1;
    gridSize = 15;
    trainSetSize = 100;
    instructionCount = 5;
    nopProbability = 0;
    instructionApplication = 0;
    task = TASK_MAJORITY;
    extendedInstructionCalculation = 0;
    taskFile = "";
    defaultDamage = 0;
    damagedConfigurationCount = 0;
    maxDamagedPercentage = 0;
    rulesetCount = 1;
    boundaryType = 1;
    rotationInstruction = 1;

    // Cellular programming settings (default)
    cellularProgramming = false;
    lockStepCount = 15;
    initialPhaseStep = 100;
    fitnessMeasuringSequence = 10;
    respectFitnessImprovement = true;
    resetGridPercentageLimit = 0.2;
    resetCellPercentageLimit = 0.5;
    extendedInstructionCalculation = false;

    configFile = "configuration.txt";
    xmlFile = "configuration.xml";
    taskFile = "";
}

Config::~Config()
{
    if(configInstance != 0)
        delete configInstance;
}

bool Config::LoadConfiguration()
{
    // Otevreni souboru
    ifstream inFile;
    inFile.open(configFile.c_str(), std::ifstream::in);
    if(inFile.fail())
    {   // Konfiguracni soubor se nepodarilo otevrit
        return false;
    }

    // Ziskani promennych z konfiguracniho souboru
    string line;
    string variableName;

    while(getline(inFile, line))
    {
        if(line.size() > 0 && isalpha(line[0]) && line.find('='))
        {
            variableName = line.substr(0, line.find('='));
            variableName.erase(variableName.find_last_not_of(' ') + 1);
            variableName.erase(0, variableName.find_first_not_of(' '));

            if(variableName == "Task")
            {
                std::string taskString = line.substr(line.find('=') + 1);
                if(taskString.find("MAJORITY") != string::npos)
                    task = TASK_MAJORITY;
                else if(taskString.find("SYNCHRONIZATION") != string::npos)
                    task = TASK_SYNCHRONIZATION;
                else if(taskString.find("SELF_ORGANIZATION") != string::npos)
                    task = TASK_SELF_ORGANIZATION;
                else if(taskString.find("COMPUTATION_ADDER") != string::npos)
                    task = TASK_COMPUTATION_ADDER;
            }
            if(variableName == "GridSize")
                gridSize = atoi(line.substr(line.find('=') + 1).c_str());
            if(variableName == "TrainSetSize")
                trainSetSize = atoi(line.substr(line.find('=') + 1).c_str());
            if(variableName == "PopulationSize")
                populationSize = atoi(line.substr(line.find('=') + 1).c_str());
            if(variableName == "GenerationCount")
                generationCount = atoi(line.substr(line.find('=') + 1).c_str());
            if(variableName == "InstructionCount")
                instructionCount = atoi(line.substr(line.find('=') + 1).c_str());
            if(variableName == "Neighbourhood")
                neighbourhood = atoi(line.substr(line.find('=') + 1).c_str());
            if(variableName == "TournamentSize")
                tournamentSize = atoi(line.substr(line.find('=') + 1).c_str());
            if(variableName == "ElitismSize")
                elitismSize = atoi(line.substr(line.find('=') + 1).c_str());
            if(variableName == "LogInterval")
                logInterval = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "AutomatonStep")
                automatonStep = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "MutationProbability")
                mutationProbability = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "EvolutionGraphValuesCount")
                evolutionGraphValuesCount = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "TimeIntervalEvolutionInstructions")
                timeIntervalEvolutionInstructions = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "TimeIntervalAnalyseInstructions")
                timeIntervalAnalyseInstructions = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "DefaultStepInterval")
                defaultStepInterval = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "NopProbability")
                nopProbability = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "InstructionApplication")
                instructionApplication = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "CellularProgramming")
                cellularProgramming = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "RespectFitnessImprovement")
                respectFitnessImprovement = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "LockStepCount")
                lockStepCount = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "InitialPhaseStep")
                initialPhaseStep = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "ResetGridPercentageLimit")
                resetGridPercentageLimit = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "ResetCellPercentageLimit")
                resetCellPercentageLimit = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "FitnessMeasuringSequence")
                fitnessMeasuringSequence = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "ExtendedInstructionCalculation")
                extendedInstructionCalculation = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "TaskFile")
                taskFile = line.substr(line.find('=') + 2);
            else if(variableName == "DefaultDamage")
                defaultDamage = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "DamagedConfigurationsCount")
                damagedConfigurationCount = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "MaxDamagedPercentage")
                maxDamagedPercentage = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "InstructionSetCount")
                rulesetCount = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "BoundaryType")
                boundaryType = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "RotationInstruction")
                rotationInstruction = atoi(line.substr(line.find('=') + 1).c_str());
        }
    }

    return true;
}

Config *Config::configInstance = 0;
