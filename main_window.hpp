#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QtCore>
#include <QtGui>

#include "renderer.hpp"
#include "stage_tab.hpp"


class main_window: public QWidget
{
    Q_OBJECT

    public:
        main_window(int force_ogl_maj, int force_ogl_min);
        ~main_window(void);

        QList<stage_tab *> stage_tabs;

    public slots:
        void add_stage(void);
        void remove_stage(int index);

    private:
        renderer *render;
        QCheckBox scale_display_fbo;
        QWidget *render_page;
        QPushButton add_stage_btn;
        QTabWidget *tabs;

        QVBoxLayout *render_layout, *main_layout;
};

#endif
