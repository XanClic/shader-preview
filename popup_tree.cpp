#include <QtGui>

#include <popup_tree.hpp>


popup_tree_widget::popup_tree_widget(QWidget *parent):
    QTreeWidget(parent),
    sel_item(NULL),
    item_popup_menu(NULL),
    bg_popup_menu(NULL)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void popup_tree_widget::mousePressEvent(QMouseEvent *evt)
{
    sel_item = itemAt(evt->pos());

    for (QTreeWidgetItem *item: selectedItems())
        item->setSelected(false);

    if (sel_item != NULL)
        sel_item->setSelected(true);

    if (evt->button() == Qt::RightButton)
    {
        QMenu *popup = (sel_item != NULL) ? item_popup_menu : bg_popup_menu;

        if (popup != NULL)
            popup->popup(evt->globalPos());
    }
}
