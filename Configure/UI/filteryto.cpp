#include "filteryto.h"
#include "Configure/configure.h"

FilterYTO::FilterYTO(QWidget *parent)
    : QGroupBox(parent)
{
    layoutMain = new QVBoxLayout;
    layoutComeCode = new QFormLayout;

    editComeCode1 = new QLineEdit;
    layoutComeCode->addRow("进港一段码:", editComeCode1);
    editComeCode1->setText(
       Configure::instance()->configureYTO->ytoCode1().join(','));
    connect(editComeCode1, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editComeCode1->text().trimmed();
        Configure::instance()->configureYTO->setConfigure("FILTER/ytoCode1Come", text.split(','));
    });

    editComeCode2 = new QLineEdit;
    layoutComeCode->addRow("进港二段码:", editComeCode2);
    editComeCode2->setText(
       Configure::instance()->configureYTO->ytoCode2().join(','));
    connect(editComeCode2, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editComeCode2->text().trimmed();
        Configure::instance()->configureYTO->setConfigure("FILTER/ytoCode2Come", text.split(','));
    });

    layoutMain->addLayout(layoutComeCode);

    setLayout(layoutMain);
}
