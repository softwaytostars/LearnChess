#include "dialogexplanation.h"
#include "ui_dialogexplanation.h"
#include <QDialogButtonBox>

DialogExplanation::DialogExplanation(const QString &aExplanation, bool aAvecModif, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExplanation)
{
    ui->setupUi(this);
    setLayout(ui->verticalLayout);
    if (aAvecModif)
        setWindowTitle(tr("Fill an explanation"));
    else
        setWindowTitle(tr("Explanation"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox (this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    ui->horizontalLayout->addWidget(buttonBox);
    setModal(true);
    ui->textEdit->setReadOnly(!aAvecModif);
    ui->textEdit->setText(aExplanation);
}

DialogExplanation::~DialogExplanation()
{
    delete ui;
}

QString DialogExplanation::getExplanation () const {

    return ui->textEdit->toPlainText ();
}

QString DialogExplanation::getExplanation (const QString& aInitExplanation, bool aAvecModifs) {

    DialogExplanation diag (aInitExplanation, aAvecModifs);
    if (diag.exec() == QDialog::Accepted) {
        return diag.getExplanation ();
    }
    else return "";
}
