#include <QtGui>

#include <texture_tree.hpp>


texture_tree::texture_tree(QWidget *parent):
    QTreeWidget(parent)
{
    popup = new QMenu;
    connect(popup->addAction("Assign to &unit..."), SIGNAL(triggered()), parent, SLOT(assign_tex()));

    setSelectionMode(QAbstractItemView::NoSelection);
}

void texture_tree::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::RightButton)
    {
        cur_item = itemAt(evt->pos());
        popup->popup(evt->globalPos());
    }
}
