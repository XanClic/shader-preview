#ifndef POPUP_TREE_HPP
#define POPUP_TREE_HPP

#include <QtGui>

class popup_tree: public QTreeWidget
{
    Q_OBJECT

    public:
        popup_tree(QWidget *parent);
        ~popup_tree(void);

        void unselect(void);

        QTreeWidgetItem *sel_item;
        QMenu *item_popup_menu, *bg_popup_menu;

    protected:
        void mousePressEvent(QMouseEvent *evt);
        void contextMenuEvent(QContextMenuEvent *evt);
};

#endif
