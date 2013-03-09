#ifndef MTK__HPANED_HPP
#define MTK__HPANED_HPP

#include <list>


namespace Mtk
{
    class HPaned
    {
        public:
            HPaned(void) { children = 0; }

            void add(Gtk::Widget &widget)
            {
                children++;

                if (children == 1)
                {
                    panes.push_back(new Gtk::HPaned);
                    panes.back()->pack1(widget, true, true);
                }
                else if (children == 2)
                    panes.back()->pack2(widget, true, true);
                else
                {
                    Gtk::Widget *old_last = panes.back()->get_child2();
                    Gtk::HPaned *new_paned = new Gtk::HPaned;

                    old_last->reference();
                    panes.back()->remove(*old_last);

                    panes.back()->pack2(*new_paned, true, true);
                    panes.push_back(new_paned);

                    new_paned->pack1(*old_last, true, true);
                    new_paned->pack2(widget,    true, true);
                }
            }

            operator Gtk::HPaned &(void) { return *panes.front(); }


        private:
            int children;
            std::list<Gtk::HPaned *> panes;
    };
}

#endif
