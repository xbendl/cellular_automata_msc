#include "analyseInstructionsTab.h"

using namespace std;

AnalyseInstructionsTab::AnalyseInstructionsTab(QWidget *parent) : QWidget(parent)
{
    config = Config::GetInstance()->configInstance;

    // Sekce "Mrizka automatu"
    palette = new Palette(this, 200, 50);
    palette->move(120, 222);
    QLabel *automatonHeader = new QLabel(tr("Automat:"), this);
    automatonHeader->setFont(QFont("Helvetica", 12, QFont::Bold));
    automatonHeader->move(340, 10);
    grid = new GridWidget(this, 234, palette);
    grid->setGeometry(340, 40, 234, 234);
    gridRepairment = new GridWidget(this, 234, palette);
    gridRepairment->setGeometry(590, 40, 234, 234);
    gridRepairment->setVisible(false);
    actualStepLabel = new QLabel("Aktualni krok: 0", this);
    actualStepLabel->setGeometry(340, 308, 250, 30);
    hammingDistanceLabel = new QLabel("Diference: -", this);
    hammingDistanceLabel->setGeometry(340, 282, 250, 30);
    automatonRepairmentHeader = new QLabel(tr("Automat (sebe-oprava):"), this);
    automatonRepairmentHeader->setFont(QFont("Helvetica", 12, QFont::Bold));
    automatonRepairmentHeader->move(590, 10);
    automatonRepairmentHeader->setVisible(false);
    actualStepRepairmentLabel= new QLabel("Aktualni krok: 0", this);
    actualStepRepairmentLabel->setGeometry(590, 308, 250, 30);
    actualStepRepairmentLabel->setVisible(false);
    hammingDistanceRepairmentLabel = new QLabel("Diference: -", this);
    hammingDistanceRepairmentLabel->setGeometry(590, 282, 250, 30);
    hammingDistanceRepairmentLabel->setVisible(false);

    // Sekce "Analyza instrukci"
    palette->SetPaletteRange(1);
    QLabel *analyseInstructionHeaderLabel = new QLabel(tr("Analyza instrukci:"), this);
    analyseInstructionHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    analyseInstructionHeaderLabel->move(10, 10);
    QLabel *instructionsLabel = new QLabel(tr("Instrukce"), this);
    instructionsLabel->move(10, 45);
    loadFileButton = new QPushButton(tr("Nacist instrukce"), this);
    loadFileButton->setGeometry(120, 40, 205, 25);
    QLabel *automatonLabel = new QLabel(tr("Automat"), this);
    automatonLabel->move(10, 75);
    automatonFileButton = new QPushButton(tr("Nacist automat"), this);
    automatonFileButton->setGeometry(120, 70, 205, 25);
    automatonFileButton->setEnabled(false);
    resetButton = new QPushButton(tr("Reset ulohy"), this);
    resetButton->setGeometry(120, 100, 205, 25);
    resetButton->setEnabled(false);
    QLabel *automatonStepLabel = new QLabel(tr("Pocet kroku"), this);
    automatonStepLabel->move(10, 135);
    automatonStepLine = new QSpinBox(this);
    automatonStepLine->setRange(1, (INT_MAX - 1));
    automatonStepLine->setGeometry(120, 130, 65, 25);
    automatonStepButton = new QPushButton(tr("Provest kroky"), this);
    automatonStepButton->setGeometry(190, 130, 135, 25);
    automatonStepButton->setEnabled(false);
    QLabel *steppingLabel = new QLabel(tr("Krokovani"), this);
    steppingLabel->move(10, 165);
    steppingLabel->setToolTip("Casovy interval mezi dvema prechody automatu; hodnota v ms");
    steppingLabel->setCursor(Qt::WhatsThisCursor);
    steppingLine = new QSpinBox(this);
    steppingLine->setRange(1, (INT_MAX - 1));
    steppingLine->setGeometry(120, 160, 65, 25);
    steppingButton = new QPushButton(tr("Spustit krokovani"), this);
    steppingButton->setGeometry(190, 160, 135, 25);
    steppingButton->setEnabled(false);
    QLabel *autoDamageLabel = new QLabel(tr("Auto poskozeni"), this);
    autoDamageLabel->setToolTip("Generovani automatickeho poskozeni v kazdem kroku; hodnota z intervalu <0;1>");
    autoDamageLabel->setCursor(Qt::WhatsThisCursor);
    autoDamageLabel->move(10, 195);
    autoDamageLine = new QDoubleSpinBox(this);
    autoDamageLine->setRange(0, 1);
    autoDamageLine->setDecimals(3);
    autoDamageLine->setSingleStep(0.001);
    autoDamageLine->setGeometry(120, 190, 65, 25);
    damageCheckBox = new QCheckBox(this);
    damageCheckBox->setGeometry(190, 190, 18, 25);
    QLabel *manualDamageLabel = new QLabel(tr("Manual poskozeni"), this);
    manualDamageLabel->setToolTip("Manualniho nastaveni poskozeni pomoci barev z palety");
    manualDamageLabel->setCursor(Qt::WhatsThisCursor);
    manualDamageLabel->move(10, 225);
    saveResultsButton = new QPushButton(tr("Ulozit"), this);
    saveResultsButton->setGeometry(120, 250, 205, 25);
    saveResultsButton->setEnabled(false);

    // Nastaveni slotu a signalu
    connect(loadFileButton, SIGNAL(clicked()), this, SLOT(SetOpenInstructionsFile()));
    connect(automatonFileButton, SIGNAL(clicked()), this, SLOT(SetOpenAutomatonFile()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(ResetTask()));
    connect(automatonStepButton, SIGNAL(clicked()), this, SLOT(ExecuteSteps()));
    connect(steppingButton, SIGNAL(clicked()), this, SLOT(ExecuteStepping()));
    connect(saveResultsButton, SIGNAL(clicked()), this, SLOT(SetSaveResultsFile()));
    connect(&analyseManager, SIGNAL(GridGenerated(Automaton::GridType, Automaton::GridType, bool)), this, SLOT(ShowGridGenerated(Automaton::GridType, Automaton::GridType, bool)));

    // Vyplneni ovladacich prvku podle nastaveni z konfiguracniho souboru
    SetConfigurationValues();

    ResetInstructionList();

    timerRunning = false;
    actualStep = 0;
    timerId = -1;

    // Nastaveni relevantnich bodu pro zjisteni fitness pro ulohu mechanickeho vypoctu
    Automaton::Point point;
    point.x = 2;
    point.y = 0;
    positionsAdder.push_back(point);
    point.x = 2;
    point.y = 1;
    positionsAdder.push_back(point);
}

bool AnalyseInstructionsTab::LoadFile(std::string inputFile, E_TASK_TYPE &taskType)
{
    automatonRepairmentHeader->setVisible(false);
    actualStepRepairmentLabel->setVisible(false);
    hammingDistanceRepairmentLabel->setVisible(false);
    gridRepairment->setVisible(false);

    resultChromosomeSet.clear();
    resultChromosomeSetRepairment.clear();
    resultChromosome.instrList.clear();
    resultChromosome.fitness = 0;

    taskType = TASK_UNKNOWN;

    // Otevreni souboru
    ifstream inFile;
    inFile.open(inputFile.c_str(), std::ifstream::in);
    if(inFile.fail())
    {   // Vstupni soubor se nepodarilo otevrit
        return false;
    }

    // Ziskani oznaceni souboru
    string line;
    getline(inFile, line);
    string fileContent = "*** INSTRUKCEMI RIZENY CELULARNI AUTOMAT ***";
    if(line.substr(0, fileContent.size()) != fileContent)
    {   // Soubor neobsahuje korektni oznaceni
        return false;
    }

    // Ziskani typu ulohy
    getline(inFile, line);
    getline(inFile, line);
    if(line.find_first_of('\r') != string::npos)
    {   // Odmazani \r z textu radku souboru
        line = line.substr(0, line.find_first_of('\r'));
    }
    if((line.find_first_of(':') != string::npos) && (line.substr(0, line.find_first_of(':')) == "Uloha"))
    {
        string taskTypeString = line.substr(line.find_first_of(':') + 1);
        if(taskTypeString.find("MAJORITY") != string::npos)
        {
            config->task = TASK_MAJORITY;
            palette->SetPaletteRange(3);
        }
        else if(taskTypeString.find("SYNCHRONIZATION") != string::npos)
        {
            config->task = TASK_SYNCHRONIZATION;
            palette->SetPaletteRange(3);
        }
        else if(taskTypeString.find("SELF_ORGANIZATION") != string::npos)
        {
            config->task = TASK_SELF_ORGANIZATION;
            palette->SetPaletteRange((ColorSampler::sampler).size());
        }
        else if(taskTypeString.find("COMPUTATION_ADDER") != string::npos)
        {
            config->task = TASK_COMPUTATION_ADDER;
            palette->SetPaletteRange(4);
        }
    }

    // Ziskani pouzivaneho okoli
    E_NEIGHBOURHOOD neighbourhoodType = NEIGHBOURHOOD_VON_NEUMANN;
    getline(inFile, line);
    if(line.find_first_of('\r') != string::npos)
    {   // Odmazani \r z textu radku souboru
        line = line.substr(0, line.find_first_of('\r'));
    }
    if((line.find_first_of(':') != string::npos) && (line.substr(0, line.find_first_of(':')) == "Okoli"))
    {
        string neighbourhoodTypeString = line.substr(line.find_first_of(':') + 1);
        if(neighbourhoodTypeString.find("VON_NEUMANN") != string::npos)
            neighbourhoodType = NEIGHBOURHOOD_VON_NEUMANN;
        else if(neighbourhoodTypeString.find("MOORE") != string::npos)
            neighbourhoodType = NEIGHBOURHOOD_MOORE;
    }

    // Ziskani uvazovaneho zpusobu prace automatu (klasicky geneticky algoritmus / celularni programovani)
    getline(inFile, line);
    if((line.find_first_of(':') != string::npos) && (line.substr(0, line.find_first_of(':')) == "Celularni programovani"))
    {
        string cellularProgrammingString = line.substr(line.find_first_of(':') + 1);
        if(cellularProgrammingString.find("TRUE") != string::npos)
            config->cellularProgramming = true;
        else
            config->cellularProgramming = false;
    }
    // Ziskani jednotlivych instrukcich zakodovanych v chromozomu
    getline(inFile, line);
    if(line.find_first_of('\r') != string::npos)
    {   // Odmazani \r z textu radku souboru
        line = line.substr(0, line.find_first_of('\r'));
    }
    if(config->cellularProgramming)
    {   // V pripade pouziti CA se jedna o mnozinu poslouponsti instrukci
        if((line.find_first_of(':') != string::npos) && (line.substr(0, line.find_first_of(':')) == "Instrukce"))
        {
            getline(inFile, line);
            line = line.substr(0, line.find_first_of('\r'));
            while(line.find_first_of("0123456789") != string::npos)
            {
                Automaton::Chromosome chromosome;
                line = line.substr(line.find_first_of(':') + 1);

                while(line.find_first_of("0123456789") != string::npos)
                {
                    line = line.substr(line.find_first_not_of(" \t"));
                    string tempString = line.substr(0, line.find_first_not_of("0123456789"));
                    int tempInt = atoi(tempString.c_str());
                    line = line.substr(line.find_first_not_of("0123456789"));
                    for(int i = 0; i < (int)instrList[config->task][neighbourhoodType].size(); i++)
                        if(instrList[config->task][neighbourhoodType][i].id == tempInt)
                            chromosome.instrList.push_back(&(instrList[taskType][neighbourhoodType][i]));
                }
                resultChromosomeSet.push_back(chromosome);
                getline(inFile, line);
            }
        }
        config->instructionCount = resultChromosomeSet[0].instrList.size();
    }
    else
    {   // V pripade pouziti klasickeho algoritmu GP se jedna o jedinou posloupnost instrukci
        if((line.find_first_of(':') != string::npos) && (line.substr(0, line.find_first_of(':')) == "Instrukce"))
        {
            line = line.substr(line.find_first_of(':') + 1);

            while(line.find_first_of("0123456789") != string::npos)
            {
                line = line.substr(line.find_first_not_of(" \t"));
                string tempString = line.substr(0, line.find_first_not_of("0123456789"));
                int tempInt = atoi(tempString.c_str());
                line = line.substr(line.find_first_not_of("0123456789"));
                for(int i = 0; i < (int)instrList[taskType][neighbourhoodType].size(); i++)
                    if(instrList[taskType][neighbourhoodType][i].id == tempInt)
                        resultChromosome.instrList.push_back(&(instrList[taskType][neighbourhoodType][i]));
            }
        }
    }
    if((config->task == TASK_SELF_ORGANIZATION) && config->cellularProgramming)
    {   // V pripade pouziti CA v uloze sebe-organizace se v souboru nachazi rovnez vysledky pro sebe-opravu
        automatonRepairmentHeader->setVisible(true);
        actualStepRepairmentLabel->setVisible(true);
        hammingDistanceRepairmentLabel->setVisible(true);
        gridRepairment->setVisible(true);

        getline(inFile, line);  // preskoceni prazdneho radku

        if((line.find_first_of(':') != string::npos) && (line.substr(0, line.find_first_of(':')) == "Instrukce (sebe-oprava)"))
        {
            getline(inFile, line);
            line = line.substr(0, line.find_first_of('\r'));
            while(line.find_first_of("0123456789") != string::npos)
            {
                Automaton::Chromosome chromosome;
                line = line.substr(line.find_first_of(':') + 1);

                while(line.find_first_of("0123456789") != string::npos)
                {
                    line = line.substr(line.find_first_not_of(" \t"));
                    string tempString = line.substr(0, line.find_first_not_of("0123456789"));
                    int tempInt = atoi(tempString.c_str());
                    line = line.substr(line.find_first_not_of("0123456789"));
                    for(int i = 0; i < (int)instrList[taskType][neighbourhoodType].size(); i++)
                        if(instrList[taskType][neighbourhoodType][i].id == tempInt)
                            chromosome.instrList.push_back(&(instrList[taskType][neighbourhoodType][i]));
                }
                resultChromosomeSetRepairment.push_back(chromosome);
                getline(inFile, line);
            }
        }
    }

    return true;
}

bool AnalyseInstructionsTab::LoadTask(std::string taskFile)
{
    zygote.clear();
    pattern.clear();
    cellPossibilities.clear();

    // Otevreni souboru
    ifstream inFile;
    inFile.open(taskFile.c_str(), std::ifstream::in);
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
    getline(inFile, line);  // Preskoceni prazneho radku

    // Ziskani rozmeru mrizky (udaj na prvnim radku souboru)
    if(getline(inFile, line))
        config->gridSize = atoi(line.c_str());

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
    Automaton::CellVector helpVector;  // Pro docasne ulozeni jednoho radku
    int helpInt;
    for(int i = 0; i < config->gridSize; i++)
    {   // Postupne nacitani radku
        getline(inFile, line);
        helpVector.clear();
        for(int j = 0; j < config->gridSize; j++)
        {
            helpInt = atoi(line.substr(j, 1).c_str());
            helpVector.push_back(helpInt);
        }
        zygote.push_back(helpVector);
    }
    getline(inFile, line);  // preskoceni prazdneho radku oddelujiciho zygotu od vzoru

    // Ziskani pozadovaneho vzoru (pro ulohu sebe-organizace)
    if((config->task == TASK_SELF_ORGANIZATION) || (config->task == TASK_COMPUTATION_ADDER))
    {
        for(int i = 0; i < config->gridSize; i++)
        {   // Postupne nacitani radku
            getline(inFile, line);
            helpVector.clear();
            for(int j = 0; j < config->gridSize; j++)
            {
                helpInt = atoi(line.substr(j, 1).c_str());
                helpVector.push_back(helpInt);
            }
            pattern.push_back(helpVector);
        }
        requiredGrid = pattern;
    }

    inFile.close();

    return true;
}

bool AnalyseInstructionsTab::SaveResults(std::string outputFile)
{
    ofstream file;
    file.open(outputFile.c_str());

    if(!file.good())
        return false;

    // Zjisteni prumerne Hammingovy vzdalenosti
    double avgHammingDistance = 0;
    double avgHammingDistanceRepairment = 0;
    for(int i = 0; i < (int)hammingDistanceStatistic.size(); i++)
    {
        avgHammingDistance += hammingDistanceStatistic[i];
        avgHammingDistanceRepairment += hammingDistanceRepairmentStatistic[i];
    }
    avgHammingDistance = ((double)avgHammingDistance / hammingDistanceStatistic.size());
    avgHammingDistanceRepairment = ((double)avgHammingDistanceRepairment / hammingDistanceStatistic.size());

    std::stringstream stepsCountOut, avgHammingDistanceOut, avgHammingDistanceRepairmentOut;
    stepsCountOut << hammingDistanceStatistic.size();
    avgHammingDistanceOut << avgHammingDistance;
    avgHammingDistanceRepairmentOut << avgHammingDistanceRepairment;

    file << "*** INSTRUKCEMI RIZENY CELULARNI AUTOMAT - VYSLEDKY ANALYZY ***\r\n\r\n";

    file << "*** SOUBORY ***\r\n";
    file << "Soubor s resenim: \t" + config->inputAnalyseFile + "\r\n";
    file << "Automat: \t" + config->inputAnalyseFile2 + "\r\n";
    file << "Prumerna diference: \t" + avgHammingDistanceOut.str() + "\r\n";
    if((config->task == TASK_SELF_ORGANIZATION) && config->cellularProgramming)
        file << "Prumerna diference (sebe-oprava): \t" + avgHammingDistanceRepairmentOut.str() + "\r\n";

    file << "\r\n*** DIFERENCE *** \r\n";
    for(unsigned int i = 0; i < hammingDistanceStatistic.size(); i++)
    {
        std::stringstream out;
        out << ((int) hammingDistanceStatistic[i]);
        file << out.str() << "\r\n";
    }

    if((config->task == TASK_SELF_ORGANIZATION) && config->cellularProgramming)
    {
        file << "\r\n*** DIFERENCE (SEBE-OPRAVA) *** \r\n";
        for(unsigned int i = 0; i < hammingDistanceRepairmentStatistic.size(); i++)
        {
            std::stringstream out;
            out << ((int) hammingDistanceRepairmentStatistic[i]);
            file << out.str() << "\r\n";
        }
    }
    file.close();

    return true;
}

//////////////////// SLOTY ////////////////////

void AnalyseInstructionsTab::SetConfigurationValues()
{
    automatonStepLine->setValue(config->defaultStepInterval);
    steppingLine->setValue(config->timeIntervalAnalyseInstructions);
    autoDamageLine->setValue(config->defaultDamage);
}

void AnalyseInstructionsTab::SetOpenAutomatonFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Vyberte soubor s automatem"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"));
    if(!fileName.isEmpty())
    {
        string taskFile = fileName.toStdString();
        config->inputAnalyseFile2 = fileName.toStdString();

        if(LoadTask(taskFile))
        {
            automatonStepButton->setEnabled(true);
            steppingButton->setEnabled(true);
            saveResultsButton->setEnabled(true);
            resetButton->setEnabled(true);
            actualStepLabel->setText("Aktualni krok: 0");
            actualStepRepairmentLabel->setText("Aktualni krok: 0");
            grid->SetGrid(zygote);
            gridRepairment->SetGrid(zygote);
            hammingDistanceStatistic.clear();    // Vynulovani statistik k Hamminove vzdalenosti
            hammingDistanceRepairmentStatistic.clear();
            actualStep = 0;

            if((config->task == TASK_MAJORITY) || (config->task == TASK_SYNCHRONIZATION))
            {
                blackGrid.clear();
                whiteGrid.clear();
                Automaton::CellVector cellVector, cellVector2;
                cellVector.assign(zygote.size(), (Automaton::CellType)1);
                cellVector2.assign(zygote.size(), (Automaton::CellType)2);
                blackGrid.assign(zygote.size(), cellVector);
                whiteGrid.assign(zygote.size(), cellVector2);
                int zeroCount = 0;

                for(int i = 0; i < (int)zygote.size(); i++)
                    for(int j = 0; j < (int)zygote.size(); j++)
                        zeroCount += (zygote[i][j] == 1) ? 1 : 0;
                if(zeroCount > ((int)(zygote.size() * zygote.size()) / 2))
                    requiredGrid = blackGrid;
                else
                    requiredGrid = whiteGrid;

                if((config->task == TASK_SYNCHRONIZATION) && (config->automatonStep % 2) == 1)
                    this->requiredGrid = (this->requiredGrid == whiteGrid) ? blackGrid : whiteGrid;
            }

            if(config->task == TASK_COMPUTATION_ADDER)
                hammingDistanceStatistic.push_back(Automaton::HammingDistanceComputationTask(zygote, requiredGrid, positionsAdder));
            else
                hammingDistanceStatistic.push_back((config->gridSize * config->gridSize) - Automaton::HammingDistanceGP(zygote, requiredGrid));

            hammingDistanceLabel->setText(QString("Diference: %1").arg(hammingDistanceStatistic.back()));
            hammingDistanceRepairmentLabel->setText(QString("Diference: %1").arg(hammingDistanceStatistic.back()));

            // Spusteni sledovani
            if(config->cellularProgramming)
                analyseManager.ObserveAutomaton(zygote, requiredGrid, resultChromosomeSet, resultChromosomeSetRepairment, cellPossibilities);
            else
                analyseManager.ObserveAutomaton(zygote, requiredGrid, resultChromosome, cellPossibilities);
        }
        else
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void AnalyseInstructionsTab::SetOpenInstructionsFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Vyberte soubor s ulozenymi instrukcemi"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"));

    if(!fileName.isEmpty())
    {
        config->inputAnalyseFile = fileName.toStdString();

        if(LoadFile(config->inputAnalyseFile, config->task))  // Nacteni ulozenych pravidel z precteneho souboru
        {
            automatonFileButton->setEnabled(true);
            automatonStepButton->setEnabled(false);
            steppingButton->setEnabled(false);
            saveResultsButton->setEnabled(false);
            resetButton->setEnabled(false);

            hammingDistanceLabel->setText(QString("Diference: 0"));
            actualStepLabel->setText(QString("Aktualni krok: 0"));
            actualStepRepairmentLabel->setText(QString("Aktualni krok: 0"));
            hammingDistanceRepairmentLabel->setText(QString("Diference: 0"));
            actualStepRepairmentLabel->setText(QString("Aktualni krok: 0"));
        }
        else
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void AnalyseInstructionsTab::SetSaveResultsFile()
{
    std::string outputFile= (QFileDialog::getSaveFileName(this, tr("Vyberte soubor pro ulozeni"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"))).toStdString();

    if(!outputFile.empty())
    {
        if(!SaveResults(outputFile))
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Ulozeni vysledku do vystupniho souboru se nezdarilo."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void AnalyseInstructionsTab::ResetTask()
{
    actualStep = 0;
    actualStepLabel->setText("Aktualni krok: 0");
    actualStepRepairmentLabel->setText("Aktualni krok: 0");
    automatonStepButton->setEnabled(true);      // Odemknuti ovladacich prvku
    steppingButton->setEnabled(true);
    saveResultsButton->setEnabled(true);
    resetButton->setEnabled(true);
    grid->SetGrid(zygote);
    gridRepairment->SetGrid(zygote);
    hammingDistanceStatistic.clear();

    if(config->task == TASK_COMPUTATION_ADDER)
        hammingDistanceStatistic.push_back(Automaton::HammingDistanceComputationTask(zygote, requiredGrid, positionsAdder));
    else
        hammingDistanceStatistic.push_back((config->gridSize * config->gridSize) - Automaton::HammingDistanceGP(zygote, requiredGrid));

    hammingDistanceLabel->setText(QString("Diference: %1").arg(hammingDistanceStatistic.back()));
    hammingDistanceRepairmentStatistic.clear();
    hammingDistanceRepairmentLabel->setText(QString("Diference: %1").arg(hammingDistanceStatistic.back()));

    if((config->task == TASK_MAJORITY) || (config->task == TASK_SYNCHRONIZATION))
    {
        int zeroCount = 0;
        for(int i = 0; i < (int)zygote.size(); i++)
            for(int j = 0; j < (int)zygote.size(); j++)
                zeroCount += (zygote[i][j] == 0) ? 1 : 0;
        if(zeroCount > ((int)(zygote.size() * zygote.size()) / 2))
            requiredGrid = blackGrid;
        else
            requiredGrid = whiteGrid;

        if((config->task == TASK_SYNCHRONIZATION) && (config->automatonStep % 2) == 1)
            this->requiredGrid = (this->requiredGrid == whiteGrid) ? blackGrid : whiteGrid;
    }
}

void AnalyseInstructionsTab::ShowGridGenerated(Automaton::GridType grid, Automaton::GridType gridRepairment, bool lastGrid)
{
    this->grid->SetGrid(grid);
    if(gridRepairment.size() > 0)
        this->gridRepairment->SetGrid(gridRepairment);

    // Zmena ovladacich prvku v zavislosti na tom, zda byl vykonan pouze jeden bezny prechod, nebo posledni prechod
    if((lastGrid == true) && !timerRunning)
    {
        loadFileButton->setEnabled(true);
        automatonStepButton->setEnabled(true);
        loadFileButton->setEnabled(true);
        steppingButton->setEnabled(true);
        saveResultsButton->setEnabled(true);
        resetButton->setEnabled(true);
    }

    // Zjisteni Hammingovy vzdalenosti mrizek automatu
    int hammingDistance = 0;
    int hammingDistanceRepairment = 0;
    if(config->task == TASK_MAJORITY)
    {
        hammingDistance = (grid.size() * grid.size()) - Automaton::HammingDistanceGP(grid, requiredGrid);
        hammingDistanceRepairment = (grid.size() * grid.size()) - Automaton::HammingDistanceGP(gridRepairment, requiredGrid);
    }
    else if(config->task == TASK_SYNCHRONIZATION)
    {
        if(requiredGrid == blackGrid)
            requiredGrid = whiteGrid;
        else
            requiredGrid = blackGrid;
        hammingDistance = (grid.size() * grid.size()) - Automaton::HammingDistanceGP(grid, requiredGrid);
        hammingDistanceRepairment = (grid.size() * grid.size()) - Automaton::HammingDistanceGP(gridRepairment, requiredGrid);
    }
    else if(config->task == TASK_SELF_ORGANIZATION)
    {
        hammingDistance = (grid.size() * grid.size()) - Automaton::HammingDistanceGP(grid, requiredGrid);
        hammingDistanceRepairment = (grid.size() * grid.size()) - Automaton::HammingDistanceGP(gridRepairment, requiredGrid);
    }
    else if(config->task == TASK_COMPUTATION_ADDER)
    {
        hammingDistance = Automaton::HammingDistanceComputationTask(grid, requiredGrid, positionsAdder);
    }

    hammingDistanceStatistic.push_back(hammingDistance);     // Ulozeni Hammingovy vzdalenosti
    hammingDistanceLabel->setText(QString("Diference: %1").arg(hammingDistance));

    hammingDistanceRepairmentStatistic.push_back(hammingDistanceRepairment);     // Ulozeni Hammingovy vzdalenosti
    hammingDistanceRepairmentLabel->setText(QString("Diference: %1").arg(hammingDistanceRepairment));

    // Zobrazeni cisla aktualniho kroku
    actualStep++;
    actualStepLabel->setText(QString("Aktualni krok: %1").arg(actualStep));
    actualStepRepairmentLabel->setText(QString("Aktualni krok: %1").arg(actualStep));
    update();

    // Krokovani je zastaveno, pokud je vypnute poskozovani a nedochazi k zmenam stavu
    if(!damageCheckBox->isChecked() && (((hammingDistanceStatistic.back() == 0) && ((hammingDistanceRepairmentStatistic.back() == 0))) || (((actualStep > 0) && ((previousGrid == grid) || (previousGridRepairment == gridRepairment))))))
    {
        if(timerId > 0)
            ExecuteStepping();
    }

    previousGrid = grid;
    previousGridRepairment = gridRepairment;
}

void AnalyseInstructionsTab::ExecuteSteps()
{
    // Zamknuti prislusnych ovladacich prvku
    loadFileButton->setEnabled(false);
    automatonStepButton->setEnabled(false);
    loadFileButton->setEnabled(false);
    steppingButton->setEnabled(false);
    saveResultsButton->setEnabled(false);
    resetButton->setEnabled(false);
    this->setFocus();

    // Kontrola vyplneni povinnych polozek
    if((automatonStepLine->text()).isEmpty())
    {
        QMessageBox *warning = new QMessageBox(QMessageBox::Warning, tr("Upozorneni"), tr("Prosim vyplnte pocet kroku."), QMessageBox::Ok, this, Qt::Widget);
        warning->show();
        return;
    }

    // Zjisteni miry pozadovaneho poskozeni
    double damageProbability = 0;
    if(damageCheckBox->isChecked())
        damageProbability = autoDamageLine->value();

    analyseManager.SetCycles(grid->GetGrid(), gridRepairment->GetGrid(), (automatonStepLine->text()).toInt(), damageProbability);
}

void AnalyseInstructionsTab::ExecuteStepping()
{
    if(timerRunning)
    {   // Vypnuti krokovani
        steppingButton->setText("Spustit krokovani");
        timerRunning = false;
        killTimer(timerId);
        timerId = -1;

        // Odemknuti ovladacich prvku
        loadFileButton->setEnabled(true);
        automatonFileButton->setEnabled(true);
        automatonStepButton->setEnabled(true);
        loadFileButton->setEnabled(true);
        saveResultsButton->setEnabled(true);
        resetButton->setEnabled(true);
    }
    else
    {   // Zapnuti krokovani
        steppingButton->setText("Zastavit krokovani");
        timerRunning = true;

        // Zamknuti ovladacich prvku
        loadFileButton->setEnabled(false);
        automatonStepButton->setEnabled(false);
        automatonFileButton->setEnabled(false);
        loadFileButton->setEnabled(false);
        saveResultsButton->setEnabled(false);
        resetButton->setEnabled(false);

        timerId = startTimer((steppingLine->text()).toInt());
    }

    update();
}

void AnalyseInstructionsTab::timerEvent(QTimerEvent *)
{
    double damageProbability = 0;
    if(damageCheckBox->isChecked())
        damageProbability = autoDamageLine->value();

    analyseManager.SetCycles(grid->GetGrid(), gridRepairment->GetGrid(), 1, damageProbability);
}

void AnalyseInstructionsTab::ResetInstructionList()
{
    for(int i = 0;  i < (int)instrList.size(); i++)
        instrList[i].clear();
    instrList.clear();

    // Nacteni instrukci z XML
    int lastId;  // Bez ucelu (jen kvuli volani fce LoadInstructions)
    XmlConfig xmlConfig;
    xmlConfig.LoadInstructions(config->xmlFile, instrList, lastId);
}
