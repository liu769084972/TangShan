#include "filtersto.h"
#include "Configure/configure.h"

FilterSTO::FilterSTO(QWidget *parent)
    : QGroupBox (parent)
{
    layoutMain = new QVBoxLayout;
    layoutComeCode = new QFormLayout;

    editComeCode1 = new QLineEdit;
    layoutComeCode->addRow("进港一段码:", editComeCode1);
    editComeCode1->setText(
       Configure::instance()->configureSTO->code1ListContain().join(','));
    connect(editComeCode1, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editComeCode1->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("FILTER/code1ListContain", text.split(','));
    });

    editComeCode2 = new QLineEdit;
    layoutComeCode->addRow("进港二段码:", editComeCode2);
    editComeCode2->setText(
       Configure::instance()->configureSTO->code2ListContain().join(','));
    connect(editComeCode2, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editComeCode2->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("FILTER/code2ListContain", text.split(','));
    });

    layoutMain->addLayout(layoutComeCode);

    setLayout(layoutMain);
}
