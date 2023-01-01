#include "instrNeighDialog.h"

using namespace std;

InstrNeighDialog::InstrNeighDialog(E_NEIGHBOURHOOD defaultNeighbourhood, QWidget *)
{
    this->setFixedSize(200, 100);
    setWindowTitle(tr("Typ instrukce"));

    QLabel *neighbourhoodLabel = new QLabel(tr("Zvolte typ instrukce:"), this);
    neighbourhoodLabel->setFont(QFont("Helvetica", 11, QFont::Normal));
    neighbourhoodLabel->move(10, 13);

    // OK button
    QPushButton *okButton = new QPushButton(tr("&OK"), this);
    okButton->setGeometry(103, 65, 87, 25);

    // Cancel button
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
    cancelButton->setGeometry(10, 65, 87, 25);

    // Neighbourhood combobox
    neighbourhoodComboBox = new QComboBox(this);
    neighbourhoodComboBox->setGeometry(10, 35, 180, 25);
    neighbourhoodComboBox->insertItem(NEIGHBOURHOOD_VON_NEUMANN, "5-okoli");
    neighbourhoodComboBox->insertItem(NEIGHBOURHOOD_MOORE, "9-okoli");
    neighbourhoodComboBox->setCurrentIndex(defaultNeighbourhood);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

E_NEIGHBOURHOOD InstrNeighDialog::GetInstructionType()
{
    return ((E_NEIGHBOURHOOD) neighbourhoodComboBox->currentIndex());
}
