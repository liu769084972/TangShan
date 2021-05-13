#ifndef HEADERVIEWCOMBOBOX_H
#define HEADERVIEWCOMBOBOX_H

#include <QHeaderView>
#include <QComboBox>

class HeaderViewComboBox : public QHeaderView
{
public:
    HeaderViewComboBox(Qt::Orientation orientation,
                       QWidget * parent = nullptr);

private:
    void paintSection(QPainter *painter, const QRect &rect,
                      int logicalIndex) const override;

private:
    QComboBox *comboLevel;
    QComboBox *comboComponent;
};

#endif // HEADERVIEWCOMBOBOX_H
