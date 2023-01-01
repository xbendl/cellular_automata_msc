#include "tabDialog.h"

TabDialog::TabDialog(QWidget *parent) : QDialog(parent)
{
    // Ziskani zadanych parametru z prikazoveho radku
    QStringList arguments = QCoreApplication::arguments();          // Pocet parametru prikazoveho radku
    bool usageFlag = false;
    int usage = 1;
    std::string inputFile, outputFile;

    for(int i = 1; i < arguments.count(); i++)
    {   // Provedeni akci na zaklade precteni jednotlivych parametru
        if((arguments[i] == "-u") && (i+1 != arguments.count()))
        {   // Precteni vyuziti programu (1 = generovani pravidel, 2 = analyza pravidel)
            usage = arguments[++i].toInt();
            usageFlag = true;
            continue;
        }
    }

    // Nastaveni ovladacich prvku podle zadaneho konfiguracniho souboru
    if(!Config::GetInstance()->LoadConfiguration())
        std::cerr << "CHYBA: Konfiguracni soubor nelze nalezt nebo neobsahuje pozadovane udaje." << std::endl;

    // Globalni nastaveni aplikace
    setFixedSize(844, 525);
    setFont(QFont("Helvetica", 9, QFont::Normal));
    setWindowTitle(tr("Instrukcemi rizeny celularni automat"));

    tabWidget = new QTabWidget(this);

    // Pridani zalozek
    evolutionInstructionsTab = new EvolutionInstructionsTab(tabWidget, this);
    analyseInstructionsTab = new AnalyseInstructionsTab(this);
    instructionTab = new InstructionTab(this);
    taskTab = new TaskTab(this);

    tabWidget->addTab(evolutionInstructionsTab, tr("Evoluce instrukci"));
    tabWidget->addTab(analyseInstructionsTab, tr("Analyza instrukci"));
    tabWidget->addTab(instructionTab, tr("Tvorba instrukci"));
    tabWidget->addTab(taskTab, tr("Tvorba uloh (sebe-organizace)"));
    tabWidget->setFixedSize(952, 725);

    // Prepnuti na sousedni zalozku v pripade, ze parametr prikazove radky udava vyuziti programu jako analyzatoru pravidel
    if(usage == 2)
        tabWidget->setCurrentIndex(1);
    else if(usage == 3)
        tabWidget->setCurrentIndex(2);
    else if(usage == 4)
        tabWidget->setCurrentIndex(3);

    // Nastaveni slotu a signalu
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(TabIndexChanged(int)));
}

void TabDialog::TabIndexChanged(int tabIndex)
{
    if(tabIndex == 0)
    {   // Pri zmene zalozky na "Evoluce instrukci" je resetovan listbox s instrukcemi (ochrana proti zmenam ze zalozky "Tvorba instrukci")
        evolutionInstructionsTab->ResetInstructionListBox();
    }
}

void TabDialog::SetTabWidgetEnabled(bool enabledFlag)
{
    tabWidget->setEnabled(enabledFlag);
}
