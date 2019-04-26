#include <QString>
#include <QTableWidget>

#ifndef TABLEWIDGETCHECKBOXITEM_HPP
#define TABLEWIDGETCHECKBOXITEM_HPP


class Table_Widget_Checkbox_Item: public QTableWidgetItem
{
    public:
    Table_Widget_Checkbox_Item(const QString &text, int type = Type);
    bool operator< (const QTableWidgetItem &other) const
    {
        return (this->checkState() > other.checkState());
    }
};



#endif // TABLEWIDGETCHECKBOXITEM_HPP
