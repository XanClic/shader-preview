#include <cstdlib>

#include <QtGui>

#include <popup_tree.hpp>


popup_tree::popup_tree(QWidget *rparent):
    QTreeWidget(rparent),
    sel_item(NULL),
    item_popup_menu(NULL),
    bg_popup_menu(NULL)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setItemsExpandable(true);
}

popup_tree::~popup_tree(void)
{
    delete item_popup_menu;
    delete bg_popup_menu;
}

void popup_tree::mousePressEvent(QMouseEvent *evt)
{
    sel_item = itemAt(evt->pos());

    for (QTreeWidgetItem *item: selectedItems())
        item->setSelected(false);

    if (sel_item != NULL)
        sel_item->setSelected(true);

    this->QTreeWidget::mousePressEvent(evt);
}

void popup_tree::contextMenuEvent(QContextMenuEvent *evt)
{
    QMenu *popup = (sel_item != NULL) ? item_popup_menu : bg_popup_menu;

    if (popup != NULL)
        popup->popup(evt->globalPos());
}

void popup_tree::unselect(void)
{
    sel_item = NULL;
    setCurrentItem(NULL);
}
