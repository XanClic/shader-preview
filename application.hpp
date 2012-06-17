#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QtCore>
#include <QtGui>

#include "gl_output.hpp"
#include "popup_tree.hpp"
#include "uniforms.hpp"


class main_window: public QWidget
{
    Q_OBJECT

    public:
        main_window(void);
        ~main_window(void);

    public slots:
        void load_vsh(void);
        void load_fsh(void);
        void save_vsh(void);
        void save_fsh(void);
        void load_tex(void);
        void apply_sh(void);
        void assign_tex(void);
        void reload_uniforms(void);
        void set_uniform_value(void);

    private:
        QPlainTextEdit *vsh_edit, *fsh_edit;
        QPushButton *sh_apply;
        popup_tree_widget *texture_widget, *uniform_widget;
        gl_output *render;
        QList<uniform *> *uniforms;
        QWidget *render_page, *config_page;
        QTabWidget *tabs;

        void load_sh(QPlainTextEdit *edit, const QString &sel_title);
        void save_sh(QPlainTextEdit *edit, const QString &sel_title);
        void update_textures(void);
};

#endif
