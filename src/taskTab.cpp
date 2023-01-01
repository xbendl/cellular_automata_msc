#include "taskTab.h"

using namespace std;

TaskTab::TaskTab(QWidget *parent) : QWidget(parent)
{
    // Sekce s mrizkami
    palette = new Palette(this, 150, 150);
    palette->move(10, 200);
    QLabel *zygoteHeaderLabel = new QLabel(tr("Zygota:"), this);
    zygoteHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    zygoteHeaderLabel->move(175, 10);
    zygoteGridWidget = new GridWidget(this, 318, palette);
    zygoteGridWidget->setGeometry(175, 40, 318, 318);
    QLabel *patternHeaderLabel = new QLabel(tr("Hledany vzor:"), this);
    patternHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    patternHeaderLabel->move(508, 10);
    patternGridWidget = new GridWidget(this, 318, palette);
    patternGridWidget->setGeometry(508, 40, 318, 318);

    // Sekce s menu - buttony
    QLabel *headerLabel = new QLabel(tr("Tvorba uloh:"), this);
    headerLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    headerLabel->move(10, 10);
    newTaskButton = new QPushButton(tr("Nova uloha"), this);
    newTaskButton->setGeometry(10, 40, 150, 25);
    loadTaskButton = new QPushButton(tr("Nacist ulohu"), this);
    loadTaskButton->setGeometry(10, 70, 150, 25);
    saveTaskButton = new QPushButton(tr("Ulozit ulohu"), this);
    saveTaskButton->setGeometry(10, 100, 150, 25);
    saveTaskButton->setEnabled(false);
    QLabel *palleteHeader = new QLabel(tr("Paleta barev:"), this);
    palleteHeader->move(10, 180);

    // Nastaveni slotu a signalu
    connect(newTaskButton, SIGNAL(clicked()), this, SLOT(SetNewTask()));
    connect(loadTaskButton, SIGNAL(clicked()), this, SLOT(SetOpenTaskFile()));
    connect(saveTaskButton, SIGNAL(clicked()), this, SLOT(SetSaveTaskFile()));
}

bool TaskTab::LoadTask(std::string inputFile, Automaton::GridType &zygoteGrid, Automaton::GridType &patternGrid)
{
    // Otevreni souboru
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
    getline(inFile, line);  // Preskoceni prazneho radku

    // Ziskani rozmeru mrizky
    int size = 0;
    if(getline(inFile, line))
        size = atoi(line.c_str());

    getline(inFile, line);  // Preskoceni radku vyjmenovavajiciho mozne stavy automatu

    // Ziskani zygoty
    Automaton::CellVector helpVector;  // Pro docasne ulozeni jednoho radku
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
        zygoteGrid.push_back(helpVector);
    }

    getline(inFile, line);  // Preskoceni prazdneho radku oddelujiciho zygotu od vzoru

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
        patternGrid.push_back(helpVector);
    }

    inFile.close();
    return true;
}


void TaskTab::SetOpenTaskFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Vyberte ulohu"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"));
    Automaton::GridType zygoteGrid;
    Automaton::GridType patternGrid;

    if(!fileName.isEmpty())
    {
        if(LoadTask(fileName.toStdString(), zygoteGrid, patternGrid))
        {
            zygoteGridWidget->SetGrid(zygoteGrid);                              // Nastaveni zygoty
            patternGridWidget->SetGrid(patternGrid);                            // Nastaveni hledaneho vzoru
            saveTaskButton->setEnabled(true);
        }
        else
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void TaskTab::SetSaveTaskFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Vyberte soubor pro ulozeni"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"));

    if(!fileName.isEmpty())
    {
        if(!SaveTask(fileName.toStdString()))
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Ulozeni ulohy do vystupniho souboru se nezdarilo."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void TaskTab::SetNewTask()
{
    bool ok;
    int size = QInputDialog::getInteger(this, tr("Zjisteni delky strany mrizky"), tr("Delka strany mrizky:"), 15, 1, 100, 1, &ok);

    if(ok)
    {
        zygoteGridWidget->SetEmptyGrid(size);
        patternGridWidget->SetEmptyGrid(size);
        saveTaskButton->setEnabled(true);
    }
}

bool TaskTab::SaveTask(std::string outputFile)
{
    string fileName;
    string fileReport;
    Automaton::GridType zygoteGrid = zygoteGridWidget->GetGrid();
    Automaton::GridType patternGrid = patternGridWidget->GetGrid();

    // Zjisteni moznych stavu mrizky
    vector<int> cellPossibilities;
    bool found;
    for(unsigned int row = 0; row < patternGrid.size(); row++)
    {
        for(unsigned int col = 0; col < patternGrid[row].size(); col++)
        {   // Pro mrizku zygoty
            found = false;
            for(unsigned int i = 0; i < cellPossibilities.size(); i++)
            {
                if(cellPossibilities[i] == zygoteGrid[row][col])
                {
                    found = true;
                    break;
                }
            }
            if(!found)
                cellPossibilities.push_back(zygoteGrid[row][col]);
        }
        for(unsigned int col = 0; col < patternGrid[row].size(); col++)
        {   // Pro mrizku hledaneho vzoru
            found = false;
            for(unsigned int i = 0; i < cellPossibilities.size(); i++)
            {
                if(cellPossibilities[i] == patternGrid[row][col])
                {
                    found = true;
                    break;
                }
            }
            if(!found)
                cellPossibilities.push_back(patternGrid[row][col]);
        }
    }

    fileReport = "*** INSTRUKCEMI RIZENY CELULARNI AUTOMAT - ULOHA ***\r\n\r\n";

    // Delka strany automatu
    std::stringstream out;
    out << zygoteGrid.size();
    fileReport += out.str() + "\r\n";

    // Tisk moznych stavu mrizky do souboru
    for(unsigned int i = 0; i < cellPossibilities.size(); i++)
    {
        std::stringstream out;
        out << ((int) cellPossibilities[i]);
        fileReport += out.str() + ";";
    }
    fileReport += "\r\n";

    // Tisk mrizky zygoty do souboru
    for(unsigned int i = 0; i < zygoteGrid.size(); i++)
    {
        for(unsigned int j = 0; j < zygoteGrid[i].size(); j++)
        {
            std::stringstream out;
            out << ((int) zygoteGrid[i][j]);
            fileReport += out.str();
        }
        fileReport += "\r\n";
    }
    fileReport += "\r\n";

    // Tisk mrizky hledaneho vzoru do souboru
    for(unsigned int i = 0; i < patternGrid.size(); i++)
    {
        for(unsigned int j = 0; j < patternGrid[i].size(); j++)
        {
            std::stringstream out;
            out << ((int) patternGrid[i][j]);
            fileReport += out.str();
        }
        fileReport += "\r\n";
    }

    std::ofstream outputFileStream(outputFile.c_str(), std::ios::out);
    std::copy(fileReport.begin(), fileReport.end(), std::ostream_iterator<char>(outputFileStream, ""));  // Zapis do souboru

    return true;
}
