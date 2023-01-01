#include "instructionTab.h"

using namespace std;

InstructionTab::InstructionTab(QWidget *)
{
    config = Config::GetInstance()->configInstance;

    // Nacteni instrukci
    XmlConfig xmlConfig;
    xmlConfig.LoadInstructions(config->xmlFile, instructionList, lastId);

    // Uloha
    QStringList taskNameList;
    taskNameList.insert(TASK_MAJORITY, "Majorita");
    taskNameList.insert(TASK_SYNCHRONIZATION, "Synchronizace");
    taskNameList.insert(TASK_SELF_ORGANIZATION, "Sebe-organizace");
    taskNameList.insert(TASK_COMPUTATION_ADDER, "Vypocty - scitacka");

    QStringListModel *taskNameModel = new QStringListModel(this);
    taskNameModel->setStringList(taskNameList);

    QLabel *taskHeaderLabel = new QLabel(tr("Uloha:"), this);
    taskHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    taskHeaderLabel->move(10, 10);

    taskListView = new InstrListView(this);
    taskListView->setModel(taskNameModel);
    taskListView->setGeometry(10, 40, 180, 180);
    taskListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QModelIndex index = taskNameModel->index(config->task);
    taskListView->setCurrentIndex(index);

    // Instrukce
    QLabel *instructionHeaderLabel = new QLabel(tr("Instrukce:"), this);
    instructionHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    instructionHeaderLabel->move(200, 10);
    instructionListView = new InstrListView(this);
    instructionListView->setGeometry(200, 40, 175, 152);
    instructionListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    neighbourhoodComboBox = new QComboBox(this);
    neighbourhoodComboBox->setGeometry(200, 195, 175, 25);
    neighbourhoodComboBox->insertItem(NEIGHBOURHOOD_VON_NEUMANN, "5-okoli");
    neighbourhoodComboBox->insertItem(NEIGHBOURHOOD_MOORE, "9-okoli");
    neighbourhoodComboBox->setCurrentIndex(config->neighbourhood);

    // Paleta
    QLabel *paletteHeaderLabel = new QLabel(tr("Paleta:"), this);
    paletteHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    paletteHeaderLabel->move(10, 460);
    palette = new Palette(this, 230, 150);
    palette->move(99, 460);

    // Okoli
    QLabel *neighbourhoodHeaderLabel = new QLabel(tr("Okoli:"), this);
    neighbourhoodHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    neighbourhoodHeaderLabel->move(10, 235);
    neighbourhoodGridWidget = new GridWidget(this, 175, palette);
    neighbourhoodGridWidget->setGeometry(10, 265, 175, 175);

    // Prechod
    QLabel *outputHeaderLabel = new QLabel(tr("Prechod:"), this);
    outputHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    outputHeaderLabel->move(200, 235);
    outputGridWidget = new GridWidget(this, 175, palette);
    outputGridWidget->setGeometry(200, 265, 175, 175);

    // Sekce s menu - buttony
    newInstructionButton = new QPushButton(tr("Nova instrukce"), this);
    newInstructionButton->setGeometry(390, 40, 150, 25);
    deleteInstructionButton = new QPushButton(tr("Smazat instrukci"), this);
    deleteInstructionButton->setGeometry(390, 70, 150, 25);
    deleteInstructionButton->setEnabled(false);
    saveInstructionButton = new QPushButton(tr("Ulozit instrukci"), this);
    saveInstructionButton->setGeometry(390, 265, 150, 25);
    saveInstructionButton->setEnabled(false);

    cellPossibilitiesCount.insert(cellPossibilitiesCount.begin() + TASK_MAJORITY, 3);
    cellPossibilitiesCount.insert(cellPossibilitiesCount.begin() + TASK_SYNCHRONIZATION, 3);
    cellPossibilitiesCount.insert(cellPossibilitiesCount.begin() + TASK_SELF_ORGANIZATION, (ColorSampler::sampler).size());
    cellPossibilitiesCount.insert(cellPossibilitiesCount.begin() + TASK_COMPUTATION_ADDER, 5);
    ActualizeInstructionList(config->task, config->neighbourhood);  // Zobrazeni seznamu instrukci prvni ulohy
    palette->SetPaletteRange(cellPossibilitiesCount[config->task]);           // Nastaveni dostupnych barev v palete (zalezi na vybrane uloze)

    // Nastaveni slotu a signalu
    connect(newInstructionButton, SIGNAL(clicked()), this, SLOT(CreateInstruction()));
    connect(deleteInstructionButton, SIGNAL(clicked()), this, SLOT(DeleteInstruction()));
    connect(saveInstructionButton, SIGNAL(clicked()), this, SLOT(SaveInstruction()));
    connect(taskListView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(TaskListViewEvent(const QModelIndex &)));
    connect(instructionListView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(InstructionListViewEvent(const QModelIndex &)));
    connect(neighbourhoodComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(NeighbourhoodComboBoxEvent(int)));
    connect(taskListView, SIGNAL(arrowKeyPressed(int)), this, SLOT(TaskListViewArrowKeyPressed(int)));
    connect(instructionListView, SIGNAL(arrowKeyPressed(int)), this, SLOT(InstrListViewArrowKeyPressed(int)));
}

void InstructionTab::TaskListViewArrowKeyPressed(int keyCode)
{
    int taskIndex = taskListView->selectionModel()->selectedIndexes()[0].row();
    int neighbourhoodIndex = neighbourhoodComboBox->currentIndex();

    switch(keyCode)
    {
        case Qt::Key_Up:
        case Qt::Key_Down:
        {
            ActualizeInstructionList(taskIndex, neighbourhoodIndex);
            palette->SetPaletteRange(cellPossibilitiesCount[taskIndex]);
            deleteInstructionButton->setEnabled(false);
            break;
        }
    }
}

void InstructionTab::InstrListViewArrowKeyPressed(int keyCode)
{
    switch(keyCode)
    {
        case Qt::Key_Up:
        case Qt::Key_Down:
        {
            QModelIndex modelIndex = taskListView->selectionModel()->selectedIndexes()[0];
            InstructionListViewEvent(modelIndex);
            break;
        }
    }
}

void InstructionTab::TaskListViewEvent(const QModelIndex &taskIndex)
{
    ActualizeInstructionList(taskIndex.row(), neighbourhoodComboBox->currentIndex());
    palette->SetPaletteRange(cellPossibilitiesCount[taskIndex.row()]);
    deleteInstructionButton->setEnabled(false);
    palette->SetColorIndex(0);
    neighbourhoodGridWidget->SetEmptyGrid();
    outputGridWidget->SetEmptyGrid();
}

void InstructionTab::InstructionListViewEvent(const QModelIndex &)
{
    int taskIndex = taskListView->selectionModel()->selectedIndexes()[0].row();
    int neighbourhoodIndex =neighbourhoodComboBox->currentIndex();
    std::vector<Automaton::Instruction>::iterator it;
    string name;

    for(it = (instructionList[taskIndex][neighbourhoodIndex]).begin(); it != (instructionList[taskIndex][neighbourhoodIndex]).end(); it++)
    {
        name = (instructionListView->selectionModel()->selectedIndexes()[0].data().toString()).toStdString();
        if(((*it).name) == name)
        {
            int output = (*it).output;

            Automaton::GridType grid = CreateGrid((*it).config);
            neighbourhoodGridWidget->SetGrid(grid);
            neighbourhoodGridWidget->SetEnabled(false);
            neighbourhoodGridWidget->SetNeighbourhood((E_NEIGHBOURHOOD)neighbourhoodIndex);

            Automaton::CellVector cellVector;
            cellVector.push_back(output);
            Automaton::GridType grid2;
            grid2.push_back(cellVector);
            outputGridWidget->SetGrid(grid2);
            outputGridWidget->SetEnabled(false);
            break;
        }
    }
    deleteInstructionButton->setEnabled(true);
}

Automaton::GridType InstructionTab::CreateGrid(Automaton::CellType configuration[])
{
    // Priprava prazdne mrizky
    Automaton::CellVector emptyVector;
    for(int i = 0; i < 3; i++)
        emptyVector.push_back(0);

    Automaton::GridType grid;
    for(int i = 0; i < 3; i++)
        grid.push_back(emptyVector);

    for(int i = 0; i < 9; i++)
        grid[i/3][i%3] = (int)configuration[i];

    return grid;
}

void InstructionTab::ActualizeInstructionList(int taskIndex, int neighbourhoodIndex)
{
    // Smazani vsech polozek (nazvu uloh) v listboxu
    QAbstractItemModel *model = instructionListView->model();
    if(model != NULL)
        model->removeRows(0, model->rowCount());

    // Vlozeni prislusnych polozek (nazvu instrukci) do listboxu
    QStringList instrNameList;
    QString actInstrName;

    vector<string> tempNameList;
    for(int i = 0; i < (int)instructionList[taskIndex][neighbourhoodIndex].size(); i++)
        if(instructionList[taskIndex][neighbourhoodIndex][i].active)
            tempNameList.push_back(instructionList[taskIndex][neighbourhoodIndex][i].name);

    sort(tempNameList.begin(), tempNameList.end());

    for(int i = 0; i < (int)tempNameList.size(); i++)
        instrNameList.append(QString(tempNameList[i].c_str()));

    QStringListModel *instrNameModel = new QStringListModel(this);
    instrNameModel->setStringList(instrNameList);
    instructionListView->setModel(instrNameModel);
}

void InstructionTab::NeighbourhoodComboBoxEvent(int neighbourhoodIndex)
{
    int selectedTaskIndex = taskListView->selectionModel()->selectedIndexes()[0].row();
    ActualizeInstructionList(selectedTaskIndex, neighbourhoodIndex);
}

void InstructionTab::CreateInstruction()
{
    E_NEIGHBOURHOOD instrNeighbourhood = NEIGHBOURHOOD_UNKNOWN;

    neighbourhoodGridWidget->SetEnabled(true);
    outputGridWidget->SetEnabled(true);

    // Dialog pro zadani typu instrukce (5okoli / 9okoli)
    InstrNeighDialog *dialog = new InstrNeighDialog((E_NEIGHBOURHOOD)neighbourhoodComboBox->currentIndex(), this);
    int result = dialog->exec();
    if(result == 1)
    {
        instrNeighbourhood = dialog->GetInstructionType();
        if(instrNeighbourhood != neighbourhoodComboBox->currentIndex())
        {
            NeighbourhoodComboBoxEvent(instrNeighbourhood);
            neighbourhoodComboBox->setCurrentIndex(instrNeighbourhood);
        }

        neighbourhoodGridWidget->SetEmptyGrid(3);
        neighbourhoodGridWidget->SetNeighbourhood(instrNeighbourhood);
        outputGridWidget->SetEmptyGrid(1);

        saveInstructionButton->setEnabled(true);
        deleteInstructionButton->setEnabled(false);
    }
}

void InstructionTab::DeleteInstruction()
{
    int taskIndex = taskListView->selectionModel()->selectedIndexes()[0].row();
    int instrIndex = instructionListView->selectionModel()->selectedIndexes()[0].row();
    int neighbourhoodIndex =neighbourhoodComboBox->currentIndex();

    // Nalezeni vybrane instrukce
    vector<Automaton::Instruction>::iterator it;
    string name;
    for(it = (instructionList[taskIndex][neighbourhoodIndex]).begin(); it != (instructionList[taskIndex][neighbourhoodIndex]).end(); it++)
    {
        name = (instructionListView->selectionModel()->selectedIndexes()[0].data().toString()).toStdString();
        if((*it).name == name)
        {
            // Oznaceni instrukce za smazanou v seznamu instrukci (datove strukture)
            (*it).active = false;

            // Smazani instrukce ze seznamu instrukci (listboxu)
            QStringListModel *list = (QStringListModel *) instructionListView->model();
            QStringList stringList = list->stringList();
            stringList.removeAt(instrIndex);
            list->setStringList(stringList);

            break;
        }
    }

    neighbourhoodGridWidget->SetEmptyGrid();
    outputGridWidget->SetEmptyGrid();

    // Ulozeni aktualni podoby datove struktury s instrukcemi
    XmlConfig xmlConfig;
    xmlConfig.SaveInstructions(config->xmlFile, instructionList, lastId);
}

void InstructionTab::SaveInstruction()
{
    Automaton::Instruction newInstr;
    int taskIndex = taskListView->selectionModel()->selectedIndexes()[0].row();
    int neighbourhoodIndex = neighbourhoodComboBox->currentIndex();
    Automaton::GridType grid = neighbourhoodGridWidget->GetGrid();

    // Prevod mrizky do pole stavu
    for(int i = 0; i < DIR_COUNT; i++)
        newInstr.config[i] = grid[i/3][i%3];

    // Ziskani korektniho jmena instrukce od uzivatele
    bool duplicityName = true;
    bool emptyInput = true;
    bool okInput;
    QString instrName;

    while(duplicityName || emptyInput)
    {
        instrName = QInputDialog::getText(this, tr("Nazev instrukce"), tr("Zvolte nazev instrukce:"), QLineEdit::Normal, tr(""), &okInput);
        if(!okInput)
            return;

        emptyInput = instrName.isEmpty();

        // Kontrola duplicity nazvu instrukce
        vector<Automaton::Instruction>::iterator it;
        string name = instrName.toStdString();
        duplicityName = false;
        for(it = (instructionList[taskIndex][neighbourhoodIndex]).begin(); it != (instructionList[taskIndex][neighbourhoodIndex]).end(); it++)
        {
            if((*it).name == name)
            {   // Kolize jmen
                duplicityName = true;
                QMessageBox::information(this, "Chybny nazev instrukce", "Zadany nazev instrukce jiz pro danou kombinaci uloha-okoli existuje. Zvolte prosim jiny nazev.");
            }
        }
    }

    // Vlozeni nove instrukce do seznamu instrukci
    QStringListModel *list = (QStringListModel *) instructionListView->model();
    QStringList stringList = list->stringList();
    int i = 0;
    for(i = 0; i < stringList.size(); i++)
        if(instrName.compare(stringList[i]) < 0)
            break;

    stringList.insert(i, instrName);
    list->setStringList(stringList);

    // Vlozeni nove instrukce do prislusne datove struktury
    newInstr.id = lastId++;
    newInstr.name = instrName.toStdString();
    newInstr.output = (outputGridWidget->GetGrid())[0][0];
    newInstr.active = true;

    instructionList[taskIndex][neighbourhoodIndex].push_back(newInstr);

    Automaton::GridType emptyGrid;
    neighbourhoodGridWidget->SetEmptyGrid();
    outputGridWidget->SetEmptyGrid();
    saveInstructionButton->setEnabled(false);

    // Ulozeni vsech instrukci do XML
    XmlConfig xmlConfig;
    xmlConfig.SaveInstructions(config->xmlFile, instructionList, lastId);
}
