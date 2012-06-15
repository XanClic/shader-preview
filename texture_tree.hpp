#ifndef TEXTURE_TREE_HPP
#define TEXTURE_TREE_HPP

#include <QtGui>

class texture_tree: public QTreeWidget
{
    Q_OBJECT

    public:
        QTreeWidgetItem *cur_item;

        texture_tree(QWidget *parent);

    protected:
        void mousePressEvent(QMouseEvent *evt);

    private:
        QMenu *popup;
};

#endif
