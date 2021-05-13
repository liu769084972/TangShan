#include "headerviewcombobox.h"

#include <QPainter>
#include <QDebug>

HeaderViewComboBox::HeaderViewComboBox(Qt::Orientation orientation,
                                       QWidget * parent)
    : QHeaderView (orientation, parent)
{
    setDefaultAlignment(Qt::AlignCenter);
    setSectionsClickable(true);

    comboLevel = new QComboBox(this);
    comboLevel->addItem("级别");
    comboLevel->addItem("一般");
    comboLevel->addItem("警告");
    comboLevel->addItem("严重");
    comboLevel->addItem("致命");

    comboComponent = new QComboBox(this);
    comboComponent->addItem("部件");
    comboComponent->addItem("小车格口");
    comboComponent->addItem("相机");
    comboComponent->addItem("本地数据库");
    comboComponent->addItem("阿里云");
}

void HeaderViewComboBox::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    qDebug() << rect << logicalIndex << __func__;
//    painter->save();
//    QHeaderView::paintSection(painter, rect, logicalIndex);
//    painter->restore();
    QStyleOptionComboBox optionCombo;
    if (logicalIndex == 2) {
        comboLevel->setGeometry(rect);
        //comboLevel->setFixedSize(rect.width(), rect.height());
        //comboLevel->move(rect.x(), rect.y());
//        optionCombo.rect = rect; //位置
//        style()->drawControl(QStyle::CE_ComboBoxLabel, &optionCombo, painter);

    } else if (logicalIndex == 3){
        comboComponent->setGeometry(rect);
//        comboComponent->set(rect.width(), rect.height());
//        comboComponent->move(rect.x(), rect.y());
//        optionCombo.rect = rect; //位置
//        style()->drawControl(QStyle::CE_ComboBoxLabel, &optionCombo, painter);
    } else {
        QHeaderView::paintSection(painter, rect, logicalIndex);
    }
}
