#include "evolutionInstructionsTab.h"

using namespace std;

EvolutionInstructionsTab::EvolutionInstructionsTab(QTabWidget *tabWidget, QWidget *parent) : QWidget(parent)
{
    this->tabWidget = tabWidget;
    config = Config::GetInstance()->configInstance;

    // Sekce "Mrizka automatu"
    evoluteChromosomeBar = new QProgressBar(this);
    evoluteChromosomeBar->setGeometry(416, 463, 400, 25);
    evoluteChromosomeBar->setMinimum(0);
    evoluteChromosomeBar->setTextVisible(false);
    QLabel *evolutionPlotHeaderLabel = new QLabel(tr("Graf vyvoje fitness:"), this);
    evolutionPlotHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    evolutionPlotHeaderLabel->move(412, 10);

    evolutionPlot = new EvolutionPlot(this, &evolutionManagerGP, &evolutionManagerCP);
    evolutionPlot->move(355, 40);
    evolutionPlot->resize(460, 410);
    evolutionPlot->show();
    projectStatus = new QLabel(tr("Evoluce instrukci"), this);
    projectStatus->setStyleSheet("font-weight: bold");
    projectStatus->setGeometry(568, 468, 300, 18);

    // Sekce "Evoluce instrukci"
    QLabel *evolutionHeaderLabel = new QLabel(tr("Nastaveni evoluce:"), this);
    evolutionHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    evolutionHeaderLabel->move(10, 10);
    QLabel *taskLabel = new QLabel(tr("Uloha *"), this);
    taskLabel->move(10, 45);
    QLabel *neighbourhoodLabel = new QLabel(tr("Okoli *"), this);
    neighbourhoodLabel->move(10, 75);
    QLabel *cellularProgrammingLabel = new QLabel(tr("Celularni programovani"), this);
    cellularProgrammingLabel->move(10, 105);
    QLabel *automatonStepLabel = new QLabel(tr("Pocet kroku v ramci generace *"), this);
    automatonStepLabel->move(10, 165);
    QLabel *populationSizeLabel = new QLabel(tr("Velikost populace *"), this);
    populationSizeLabel->move(10, 135);
    QLabel *generationCountStepLabel = new QLabel(tr("Pocet generaci *"), this);
    generationCountStepLabel->move(10, 195);
    QLabel *mutationProbabilityLabel = new QLabel(tr("Pravdepodobnost mutace *"), this);
    mutationProbabilityLabel->move(10, 225);
    QLabel *gridSizeLabel = new QLabel(tr("Rozmer trenovacich vzorku *"), this);
    gridSizeLabel->move(10, 255);
    QLabel *trainSetLabel = new QLabel(tr("Pocet trenovacich vzorku *"), this);
    trainSetLabel->move(10, 285);
    QLabel *instructionPoolLabel = new QLabel(tr("Mnozina pripustnych instrukci"), this);
    instructionPoolLabel->move(10, 315);

    QLabel *instructionSelectionLabel = new QLabel(tr("Vybrane instrukce"), this);
    instructionSelectionLabel->move(215, 315);
    taskComboBox = new QComboBox(this);
    taskComboBox->addItem("Majorita");
    taskComboBox->addItem("Synchronizace");
    taskComboBox->addItem("Sebe-organizace");
    taskComboBox->addItem("Vypocty - Scitacka");
    taskComboBox->setGeometry(145, 40, 180, 25);
    neighbourhoodComboBox = new QComboBox(this);
    neighbourhoodComboBox->insertItem(NEIGHBOURHOOD_VON_NEUMANN, "von Neumannovo (5-okoli)");
    neighbourhoodComboBox->insertItem(NEIGHBOURHOOD_MOORE, "Moorovo (9-okoli)");
    neighbourhoodComboBox->setGeometry(145, 70, 180, 25);
    cellularProgrammingCheckbox = new QCheckBox(this);
    cellularProgrammingCheckbox->setGeometry(215, 100, 25, 25);
    populationSizeStepLine = new QSpinBox(this);
    populationSizeStepLine->setRange(1, (INT_MAX - 1));
    populationSizeStepLine->setGeometry(215, 130, 110, 25);
    automatonStepLine = new QSpinBox(this);
    automatonStepLine->setRange(1, (INT_MAX - 1));
    automatonStepLine->setGeometry(215, 160, 110, 25);
    generationCountStepLine = new QSpinBox(this);
    generationCountStepLine->setRange(1, (INT_MAX - 1));
    generationCountStepLine->setGeometry(215, 190, 110, 25);
    mutationProbabilityLine = new QDoubleSpinBox(this);
    mutationProbabilityLine->setRange(0, 1);
    mutationProbabilityLine->setDecimals(3);
    mutationProbabilityLine->setSingleStep(0.001);
    mutationProbabilityLine->setGeometry(215, 220, 110, 25);
    gridSizeLine = new QSpinBox(this);
    gridSizeLine->setRange(1, (INT_MAX - 1));
    gridSizeLine->setGeometry(215, 250, 110, 25);
    trainSetLine = new QSpinBox(this);
    trainSetLine->setRange(1, (INT_MAX - 1));
    trainSetLine->setGeometry(215, 280, 110, 25);
    evolutionInstructionButton = new QPushButton(tr("Start"), this);
    evolutionInstructionButton->setGeometry(10, 462, 73, 25);
    stopEvolutionInstructionButton = new QPushButton(tr("Pozastavit"), this);
    stopEvolutionInstructionButton->setGeometry(88, 462, 73, 25);
    stopEvolutionInstructionButton->setEnabled(false);
    abortEvolutionInstructionButton = new QPushButton(tr("Zrusit"), this);
    abortEvolutionInstructionButton->setGeometry(166, 462, 74, 25);
    abortEvolutionInstructionButton->setEnabled(false);
    saveChromosomeButton = new QPushButton(tr("Ulozit"), this);
    saveChromosomeButton->setGeometry(245, 462, 80, 25);
    saveChromosomeButton->setEnabled(false);

    instructionPool = new InstrListView(this);
    instructionPool->setGeometry(10, 340, 110, 110);
    instructionPool->setEditTriggers(QAbstractItemView::NoEditTriggers);
    instructionPool->setSelectionMode(QAbstractItemView::ExtendedSelection);

    instructionSelection = new InstrListView(this);
    instructionSelection->setGeometry(215, 340, 110, 110);
    instructionSelection->setEditTriggers(QAbstractItemView::NoEditTriggers);
    instructionSelection->setSelectionMode(QAbstractItemView::ExtendedSelection);

    addInstructionButton = new QPushButton(tr(">>"), this);
    addInstructionButton->setGeometry(132, 365, 70, 25);
    subtractInstructionButton = new QPushButton(tr("<<"), this);
    subtractInstructionButton->setGeometry(132, 395, 70, 25);

    qRegisterMetaType<Automaton::GridType>("Automaton::GridType");

    // Nastaveni slotu a signalu
    connect(addInstructionButton, SIGNAL(clicked()), this, SLOT(AddInstructionButtonClicked()));
    connect(subtractInstructionButton, SIGNAL(clicked()), this, SLOT(SubtractInstructionButtonClicked()));
    connect(evolutionInstructionButton, SIGNAL(clicked()), this, SLOT(StartEvoluteInstructions()));
    connect(stopEvolutionInstructionButton, SIGNAL(clicked()), this, SLOT(StopEvoluteInstructions()));
    connect(abortEvolutionInstructionButton, SIGNAL(clicked()), this, SLOT(AbortEvoluteInstructions()));
    connect(saveChromosomeButton, SIGNAL(clicked()), this, SLOT(SetSaveChromosomeFile()));
    connect(taskComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(TaskIndexChanged()));
    connect(neighbourhoodComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(NeighbourhoodIndexChanged()));
    connect(cellularProgrammingCheckbox, SIGNAL(stateChanged(int)), this, SLOT(CellularProgrammingStateChanged()));
    connect(taskComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(TaskTypeChanged(int)));
    connect(&evolutionManagerGP, SIGNAL(CurrentChromosomeGenerated(float, float)), this, SLOT(ReceiveCurrentChromosomeGenerated()));
    connect(&evolutionManagerGP, SIGNAL(ChromosomeGenerated(bool)), this, SLOT(ReceiveChromosomeGenerated(bool)));
    connect(&evolutionManagerCP, SIGNAL(CurrentChromosomeGenerated(float, float)), this, SLOT(ReceiveCurrentChromosomeGenerated()));
    connect(&evolutionManagerCP, SIGNAL(ChromosomeGenerated(bool)), this, SLOT(ReceiveChromosomeGenerated(bool)));

    // Vyplneni ovladacich prvku podle nastaveni z konfiguracniho souboru
    SetConfigurationValues();

    // Naplneni instructionPool a instructionSelection
    ResetInstructionListBox();

    CellularProgrammingStateChanged();
    TaskTypeChanged(config->task);

    // Vynulovani promennych pro ulozeni vysledku simulace
    this->timeSummary = 0;
}

void EvolutionInstructionsTab::CellularProgrammingStateChanged()
{
    if(cellularProgrammingCheckbox->checkState() == Qt::Checked)
        populationSizeStepLine->setEnabled(false);
    else
        populationSizeStepLine->setEnabled(true);
}

void EvolutionInstructionsTab::TaskTypeChanged(int actType)
{
    if(actType == TASK_SELF_ORGANIZATION)
        gridSizeLine->setEnabled(false);
    else
        gridSizeLine->setEnabled(true);
}

void EvolutionInstructionsTab::TaskIndexChanged()
{
    ResetInstructionListBox();
}

void EvolutionInstructionsTab::NeighbourhoodIndexChanged()
{
    ResetInstructionListBox();
}

void EvolutionInstructionsTab::AddInstructionButtonClicked()
{
    // Presun vybrane polozky z mnoziny vsech instrukci do mnoziny vybranych instrukci (instructionPool -> instructionSelection)
    if((instructionPool->selectionModel() != NULL) && (instructionPool->selectionModel()->selectedIndexes().size() > 0))
    {
        // Odstraneni vybrane polozky z mnoziny vsech instrukci (instructionPool) a pridani do mnoziny vybranych instrukci (instructionSelection)
        vector<string> selItems;
        QString instrName;
        QStringListModel *instrSelModel;

        if(instructionSelection->model() != NULL)
            instrSelModel = (QStringListModel *)instructionSelection->model();
        else
            instrSelModel = new QStringListModel();

        QStringList instrSelList = instrSelModel->stringList();

        QModelIndexList indexes;
        QAbstractItemModel *instrPoolModel = instructionPool->model();
        while((indexes = instructionPool->selectionModel()->selectedIndexes()).size())
        {
            instrName = instructionPool->selectionModel()->selectedIndexes()[0].data().toString();
            instrSelList.append(instrName);
            selItems.push_back(instrName.toStdString());
            instrPoolModel->removeRow(indexes.first().row());
        }
        instrSelModel->setStringList(instrSelList);
        instructionSelection->setModel(instrSelModel);

        // Vlozeni vybrane polozky do datove struktury vybranych polozek (selInstrList)
        vector<Automaton::Instruction>::iterator it;
        int taskIndex = taskComboBox->currentIndex();
        int neighbourhoodIndex = neighbourhoodComboBox->currentIndex();
        string name;

        for(int i = 0; i < (int)selItems.size(); i++)
        {
            for(it = (instrList[taskIndex][neighbourhoodIndex]).begin(); it != (instrList[taskIndex][neighbourhoodIndex]).end(); it++)
            {
                if((*it).name == selItems[i])
                {
                    selInstrList.push_back(*it);

                    // Smazani instrukce ze seznamu instrukci (datove struktury)
                    (instrList[taskIndex][neighbourhoodIndex]).erase(it);
                    break;
                }
            }
        }
    }
}

void EvolutionInstructionsTab::SubtractInstructionButtonClicked()
{
    // Presun vybrane polozky z mnoziny vybranych instrukci do mnoziny vsech instrukci (instructionPool -> instructionSelection)
    if((instructionSelection->selectionModel() != NULL) && (instructionSelection->selectionModel()->selectedIndexes().size() > 0))
    {
        // Odstraneni vybrane polozky z mnoziny vsech instrukci (instructionPool) a pridani do mnoziny vybranych instrukci (instructionSelection)
        vector<string> selItems;
        QString instrName;
        QStringListModel *instrPoolModel;

        if(instructionPool->model() != NULL)
            instrPoolModel = (QStringListModel *)instructionPool->model();
        else
            instrPoolModel = new QStringListModel();

        QStringList instrPoolList = instrPoolModel->stringList();

        QModelIndexList indexes;
        QAbstractItemModel *instrSelModel = instructionSelection->model();
        while((indexes = instructionSelection->selectionModel()->selectedIndexes()).size())
        {
            instrName = instructionSelection->selectionModel()->selectedIndexes()[0].data().toString();
            instrPoolList.append(instrName);
            selItems.push_back(instrName.toStdString());
            instrSelModel->removeRow(indexes.first().row());
        }
        instrPoolModel->setStringList(instrPoolList);
        instructionPool->setModel(instrPoolModel);

        // Odstraneni vybrane polozky z datove struktury vybranych polozek (selInstrList)
        vector<Automaton::Instruction>::iterator it;
        for(int i = 0; i < (int)selItems.size(); i++)
        {
            for(it = selInstrList.begin(); it != selInstrList.end(); it++)
            {
                if((*it).name == selItems[i])
                {
                    selInstrList.erase(it);
                    break;
                }
            }
        }
    }
}

void EvolutionInstructionsTab::ResetInstructionListBox()
{
    int taskIndex = taskComboBox->currentIndex();
    int neighbourhoodIndex = neighbourhoodComboBox->currentIndex();

    for(int i = 0;  i < (int)instrList.size(); i++)
        instrList[i].clear();
    instrList.clear();

    selInstrList.clear();

    // Smazani vsech polozek (nazvu uloh) v listview s mnozinou pripustnych instrukci
    QAbstractItemModel *model = instructionPool->model();
    if(model != NULL)
        model->removeRows(0, model->rowCount());

    // Smazani vsech polozek (nazvu uloh) v listboxu s mnozinou vybranych instrukci
    QAbstractItemModel *model2 = instructionSelection->model();
    if(model2 != NULL)
        model2->removeRows(0, model2->rowCount());

    int lastId;
    XmlConfig xmlConfig;
    xmlConfig.LoadInstructions(config->xmlFile, instrList, lastId);

    // Box s vyberem ze vsech instrukci pro danou ulohu a okoli
    vector<Automaton::Instruction>::iterator it;
    QStringList instructionPoolList;
    vector<string> tempNameList;
    for(it = (instrList[taskIndex][neighbourhoodIndex]).begin(); it != (instrList[taskIndex][neighbourhoodIndex]).end(); it++)
        if((*it).active)
            tempNameList.push_back((*it).name);

    sort(tempNameList.begin(), tempNameList.end());

    for(int i = 0; i < (int)tempNameList.size(); i++)
        instructionPoolList.append(QString(tempNameList[i].c_str()));

    QStringListModel *instructionPoolNameModel = new QStringListModel(this);
    instructionPoolNameModel->setStringList(instructionPoolList);

    instructionPool->setModel(instructionPoolNameModel);
}

void EvolutionInstructionsTab::SetConfigurationValues()
{
    if((E_NEIGHBOURHOOD)config->neighbourhood == NEIGHBOURHOOD_VON_NEUMANN)
        neighbourhoodComboBox->setCurrentIndex(0);
    else
        neighbourhoodComboBox->setCurrentIndex(1);

    if(config->cellularProgramming)
        cellularProgrammingCheckbox->setCheckState(Qt::Checked);
    else
        cellularProgrammingCheckbox->setCheckState(Qt::Unchecked);

    if(cellularProgrammingCheckbox->checkState() == Qt::Checked)
        populationSizeStepLine->setEnabled(true);
    else
        populationSizeStepLine->setEnabled(false);

    taskComboBox->setCurrentIndex((int)config->task);
    automatonStepLine->setValue(config->automatonStep);
    populationSizeStepLine->setValue(config->populationSize);
    generationCountStepLine->setValue(config->generationCount);
    mutationProbabilityLine->setValue(config->mutationProbability);
    gridSizeLine->setValue(config->gridSize);
    trainSetLine->setValue(config->trainSetSize);
}

void EvolutionInstructionsTab::SetSaveChromosomeFile()
{
    std::string outputFile= (QFileDialog::getSaveFileName(this, tr("Vyberte soubor pro ulozeni"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"))).toStdString();

    if(!outputFile.empty())
    {
        if((config->cellularProgramming) && !(evolutionManagerCP.SaveChromosome(outputFile)))
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Ulozeni chromozomu s instrukcemi do vystupniho souboru se nezdarilo.\nMozne priciny: Evoluce nebyla dokoncena."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
        else if(!(config->cellularProgramming) && !(evolutionManagerGP.SaveChromosome(outputFile)))
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Ulozeni chromozomu s instrukcemi do vystupniho souboru se nezdarilo.\nMozne priciny: Evoluce nebyla dokoncena."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void EvolutionInstructionsTab::StartEvoluteInstructions()
{
    if((int)selInstrList.size() < (config->instructionCount + 1))
    {   // Mnozina vybranych instrukci je prazdna -> nelze zacit vypocet
        QMessageBox *error = new QMessageBox(QMessageBox::Warning, tr("Upozorneni"), tr("Mnozina vybranych instrukci je mensi nez delka chromozomu. Vyberte vice instrukci."), QMessageBox::Ok, this, Qt::Widget);
        error->show();
        return;
    }

    evolutionPlot->ResetTimer();    // Aktivace casovace pro beh grafu vyvoje fitness
    evolutionPlot->StartTimer();

    // Kontrola vyplneni povinnych polozek
    if((automatonStepLine->text()).isEmpty() || (mutationProbabilityLine->text()).isEmpty() || (generationCountStepLine->text()).isEmpty() || (populationSizeStepLine->text()).isEmpty())
    {
        QMessageBox *error = new QMessageBox(QMessageBox::Warning, tr("Upozorneni"), tr("Nejsou vyplnene vsechny povinne polozky (polozky oznacene hvezdickou)."), QMessageBox::Ok, this, Qt::Widget);
        error->show();
        return;
    }

    // Odemknuti / zamknuti ovladacich prvku
    evolutionInstructionButton->setEnabled(false);
    stopEvolutionInstructionButton->setEnabled(true);
    abortEvolutionInstructionButton->setEnabled(true);
    saveChromosomeButton->setEnabled(false);
    this->tabWidget->setTabEnabled(1, false);
    this->tabWidget->setTabEnabled(2, false);

    projectStatus->setText("Evoluce instrukci");
    projectStatus->setGeometry(568, 468, 300, 18);

    // Aktualizace konfiguracnich udaju podle zmen uzivatele primo ve formulari
    config->neighbourhood = neighbourhoodComboBox->currentText().toInt();
    if(neighbourhoodComboBox->currentIndex() == (int)NEIGHBOURHOOD_VON_NEUMANN)
        config->neighbourhood = NEIGHBOURHOOD_VON_NEUMANN;
    else
        config->neighbourhood = NEIGHBOURHOOD_MOORE;
    config->gridSize = gridSizeLine->text().toInt();
    config->trainSetSize = trainSetLine->text().toInt();
    config->task = (E_TASK_TYPE)taskComboBox->currentIndex();
    config->automatonStep = automatonStepLine->text().toInt();
    config->generationCount = generationCountStepLine->text().toInt();
    config->populationSize = populationSizeStepLine->text().toInt();
    config->mutationProbability = mutationProbabilityLine->text().toDouble();
    if(cellularProgrammingCheckbox->checkState() == Qt::Checked)
        config->cellularProgramming = true;
    else
        config->cellularProgramming = false;

    // Nastaveni progressBaru
    evoluteChromosomeBar->setValue(evoluteChromosomeBar->minimum());
    evoluteChromosomeBar->setMaximum(config->generationCount+1);

    // Vynulovani pripadnych vysledku z minulych behu
    this->timeSummary = 0;

    if(config->task == TASK_SELF_ORGANIZATION)
        config->cellularProgramming = true;

    // Spusteni evoluce
    if(config->cellularProgramming)
        evolutionManagerCP.FindSolution(selInstrList);
    else
        evolutionManagerGP.FindSolution(selInstrList);
}

void EvolutionInstructionsTab::StopEvoluteInstructions()
{
    if(stopEvolutionInstructionButton->text() == "Pozastavit")   // Stridani textu na buttonu Pozastavit / Obnovit
       stopEvolutionInstructionButton->setText("Obnovit");
    else
       stopEvolutionInstructionButton->setText("Pozastavit");

    // Pozastaveni evoluce
    if(config->cellularProgramming)
        evolutionManagerCP.StopEvoluteInstructions();
    else
        evolutionManagerGP.StopEvoluteInstructions();

    if(evolutionPlot->IsTimerRunning())     // Zastaveni grafu zobrazujiciho vyvoj fitness
        evolutionPlot->KillTimer(true);
    else
        evolutionPlot->StartTimer();
}

void EvolutionInstructionsTab::AbortEvoluteInstructions()
{
    // Odemknuti ovladacich prvku
    evolutionInstructionButton->setEnabled(true);
    stopEvolutionInstructionButton->setEnabled(false);
    abortEvolutionInstructionButton->setEnabled(false);
    saveChromosomeButton->setEnabled(true);
    evolutionPlot->KillTimer(true);
    this->tabWidget->setTabEnabled(1, true);
    this->tabWidget->setTabEnabled(2, true);
    this->tabWidget->setTabEnabled(3, true);

    // Pozastaveni evoluce
    if(config->cellularProgramming)
        evolutionManagerCP.AbortEvoluteInstructions();
    else
        evolutionManagerGP.AbortEvoluteInstructions();
}

void EvolutionInstructionsTab::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(QPen(Qt::gray, 1));
    painter.drawLine(340, 40, 340, 485);
}

void EvolutionInstructionsTab::ReceiveCurrentChromosomeGenerated()
{
    evoluteChromosomeBar->setValue(evoluteChromosomeBar->value() + 1);
}

void EvolutionInstructionsTab::ReceiveChromosomeGenerated(bool basicInstructionSet)
{
    // Opetovne zapnuti casovace (v pripade, ze se jedna o ulohy sebeorganizace a budou se generovat intrukce pro sebeopravu)
    if((config->task == TASK_SELF_ORGANIZATION) && config->cellularProgramming && basicInstructionSet)
    {
        evolutionPlot->ResetTimer();
        evolutionPlot->StartTimer();
        projectStatus->setText("Evoluce instrukci (sebeoprava)");
        projectStatus->setGeometry(530, 468, 300, 18);
        evoluteChromosomeBar->setValue(0);
        update();
        return;
    }
    else
        evolutionPlot->KillTimer(false);

    // Odemknuti ovladacich prvku
    evolutionInstructionButton->setEnabled(true);
    stopEvolutionInstructionButton->setEnabled(false);
    abortEvolutionInstructionButton->setEnabled(false);
    saveChromosomeButton->setEnabled(true);
    evoluteChromosomeBar->setValue(evoluteChromosomeBar->maximum());
    this->tabWidget->setTabEnabled(1, true);
    this->tabWidget->setTabEnabled(2, true);
    this->tabWidget->setTabEnabled(3, true);

    update();
}
