#ifndef POPUP_TREE_HPP
#define POPUP_TREE_HPP

#include <QtGui>

class popup_tree: public QTreeWidget
{
    Q_OBJECT

    public:
        QTreeWidgetItem *sel_item;
        QMenu *item_popup_menu, *bg_popup_menu;

        popup_tree(QWidget *parent);
        ~popup_tree(void);

    protected:
        void mousePressEvent(QMouseEvent *evt);
        void contextMenuEvent(QContextMenuEvent *evt);
};

#endif
