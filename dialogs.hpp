#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include <QtGui>

#include "texture_management.hpp"
#include "trackable.hpp"
#include "types.hpp"
#include "uniform.hpp"
#include "vertex_info.hpp"


class vector_dialog: public QDialog
{
    Q_OBJECT

    public:
        vector_dialog(QWidget *parent, const QString &title, const QString &text, int dim);
        ~vector_dialog(void);

        static vec2 get_vec2(QWidget *parent, const QString &title, const QString &text, const vec2 &init = vec2(0.f, 0.f), bool *ok = NULL)
        {
            vector_dialog dial(parent, title, text, 2);

            dial.spin_boxes[0].setValue(init.x);
            dial.spin_boxes[1].setValue(init.y);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            return vec2(dial.spin_boxes[0].value(), dial.spin_boxes[1].value());
        }

        static vec3 get_vec3(QWidget *parent, const QString &title, const QString &text, const vec3 &init = vec3(0.f, 0.f, 0.f), bool *ok = NULL)
        {
            vector_dialog dial(parent, title, text, 3);

            dial.spin_boxes[0].setValue(init.x);
            dial.spin_boxes[1].setValue(init.y);
            dial.spin_boxes[2].setValue(init.z);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            return vec3(dial.spin_boxes[0].value(), dial.spin_boxes[1].value(), dial.spin_boxes[2].value());
        }

        static vec4 get_vec4(QWidget *parent, const QString &title, const QString &text, const vec4 &init = vec4(0.f, 0.f, 0.f, 0.f), bool *ok = NULL)
        {
            vector_dialog dial(parent, title, text, 4);

            dial.spin_boxes[0].setValue(init.x);
            dial.spin_boxes[1].setValue(init.y);
            dial.spin_boxes[2].setValue(init.z);
            dial.spin_boxes[3].setValue(init.w);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            return vec4(dial.spin_boxes[0].value(), dial.spin_boxes[1].value(), dial.spin_boxes[2].value(), dial.spin_boxes[3].value());
        }


    public slots:
        void get_color(void);


    private:
        QLabel *text_label;
        QDoubleSpinBox *spin_boxes;
        QPushButton *color_button, *accept_button, *reject_button;
        QHBoxLayout *coord_layout, *button_layout;
        QVBoxLayout *top_layout;

        int dimension;
};


class matrix_dialog: public QDialog
{
    Q_OBJECT

    public:
        matrix_dialog(QWidget *parent, const QString &title, const QString &text, int dim);
        ~matrix_dialog(void);

        static mat2 get_mat2(QWidget *parent, const QString &title, const QString &text, const mat2 &init = mat2(vec2(1.f, 0.f), vec2(0.f, 1.f)), bool *ok = NULL)
        {
            matrix_dialog dial(parent, title, text, 2);

            for (int i = 0; i < 4; i++)
                dial.spin_boxes[i].setValue(init.d[i]);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            mat2 ret;
            for (int i = 0; i < 4; i++)
                ret.d[i] = dial.spin_boxes[i].value();

            return ret;
        }

        static mat3 get_mat3(QWidget *parent, const QString &title, const QString &text, const mat3 &init = mat3(vec3(1.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f)), bool *ok = NULL)
        {
            matrix_dialog dial(parent, title, text, 3);

            for (int i = 0; i < 9; i++)
                dial.spin_boxes[i].setValue(init.d[i]);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            mat3 ret;
            for (int i = 0; i < 9; i++)
                ret.d[i] = dial.spin_boxes[i].value();

            return ret;
        }

        static mat4 get_mat4(QWidget *parent, const QString &title, const QString &text, const mat4 &init = mat4(vec4(1.f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f), vec4(0.f, 0.f, 0.f, 1.f)), bool *ok = NULL)
        {
            matrix_dialog dial(parent, title, text, 4);

            for (int i = 0; i < 16; i++)
                dial.spin_boxes[i].setValue(init.d[i]);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            mat4 ret;
            for (int i = 0; i < 16; i++)
                ret.d[i] = dial.spin_boxes[i].value();

            return ret;
        }


    private:
        QLabel *text_label;
        QDoubleSpinBox *spin_boxes;
        QPushButton *accept_button, *reject_button;
        QHBoxLayout *coord_layouts, *button_layout;
        QVBoxLayout *top_layout;

        int dimension;
};


class texture_dialog: public QDialog
{
    Q_OBJECT

    public:
        texture_dialog(QWidget *parent, const QString &title, const QString &text);
        ~texture_dialog(void);

        static managed_texture *get_tex(QWidget *parent, const QString &title, const QString &text, bool *ok = NULL)
        {
            texture_dialog dial(parent, title, text);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            return static_cast<managed_texture *>(dial.choose_box->itemData(dial.choose_box->currentIndex()).value<void *>());
        }


    public slots:
        void load_tex(void);


    private:
        QLabel *text_label;
        QComboBox *choose_box;
        QPushButton *load_button, *accept_button, *reject_button;
        QHBoxLayout *button_layout;
        QVBoxLayout *top_layout;
};


class trackable_dialog: public QDialog
{
    Q_OBJECT

    public:
        trackable_dialog(QWidget *parent, const QString &title, const QString &text, uniform::utype type);
        ~trackable_dialog(void);

        static trackable *get_track(QWidget *parent, const QString &title, const QString &text, uniform::utype type, bool *ok = NULL)
        {
            trackable_dialog dial(parent, title, text, type);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            int idx = dial.choose_box->currentIndex();
            if (idx < 0)
            {
                if (ok != NULL)
                    *ok = false;
                return NULL;
            }

            return static_cast<trackable *>(dial.choose_box->itemData(idx).value<void *>());
        }


    private:
        QLabel *text_label;
        QComboBox *choose_box;
        QPushButton *accept_button, *reject_button;
        QHBoxLayout *button_layout;
        QVBoxLayout *top_layout;
};


struct wavefront_assignment
{
    vertex_attrib *va;
    bool fixed;
};

struct wavefront_fixed_assignment_float: public wavefront_assignment
{
    float val;
};

struct wavefront_fixed_assignment_vec2: public wavefront_assignment
{
    vec2 val;
};

struct wavefront_fixed_assignment_vec3: public wavefront_assignment
{
    vec3 val;
};

struct wavefront_fixed_assignment_vec4: public wavefront_assignment
{
    vec4 val;
};

struct wavefront_var_assignment: public wavefront_assignment
{
    enum list
    {
        wf_vertices = 0,
        wf_texcoords = 1,
        wf_normals = 2
    } val;
};


class wavefront_load_dialog: public QDialog
{
    Q_OBJECT

    public:
        wavefront_load_dialog(QWidget *parent, const QString &title, const QString &text, const QList<vertex_attrib *> &vas);
        ~wavefront_load_dialog(void);

        static QList<wavefront_assignment *> *get_assignment(QWidget *parent, const QString &title, const QString &text, const QList<vertex_attrib *> &vas, bool *ok = NULL)
        {
            wavefront_load_dialog dial(parent, title, text, vas);

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            return dial.assignments;
        }


    public slots:
        void va_changed(int ni);
        void asgn_changed(int ni);
        void val_changed(double d);


    private:
        bool ignore_asgn;
        QLabel *text_label;
        QComboBox *va_box, *asgn_box;
        QDoubleSpinBox *spin_boxes;
        QPushButton *accept_button, *reject_button;
        QHBoxLayout *button_layout, *coord_layout;
        QVBoxLayout *top_layout;
        QList<wavefront_assignment *> *assignments; // not destroyed in the destructor
};

#endif
