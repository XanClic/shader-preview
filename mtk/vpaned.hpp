#ifndef MTK__VPANED_HPP
#define MTK__VPANED_HPP

#include <list>


namespace Mtk
{
    class VPaned
    {
        public:
            VPaned(void) { children = 0; }

            void add(Gtk::Widget &widget)
            {
                children++;

                if (children == 1)
                {
                    panes.push_back(new Gtk::VPaned);
                    panes.back()->pack1(widget, true, false);
                }
                else if (children == 2)
                    panes.back()->pack2(widget, true, false);
                else
                {
                    Gtk::Widget *old_last = panes.back()->get_child2();
                    Gtk::VPaned *new_paned = new Gtk::VPaned;

                    old_last->reference();
                    panes.back()->remove(*old_last);

                    panes.back()->pack2(*new_paned, true, false);
                    panes.push_back(new_paned);

                    new_paned->pack1(*old_last, true, false);
                    new_paned->pack2(widget,    true, false);
                }
            }

            Gtk::VPaned &gtk(void) { return *panes.front(); }

            operator Gtk::VPaned &(void) { return *panes.front(); }


        private:
            int children;
            std::list<Gtk::VPaned *> panes;
    };
}

#endif
