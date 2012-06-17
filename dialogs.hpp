#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include <QtGui>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>


class vector_dialog: public QDialog
{
    Q_OBJECT

    public:
        vector_dialog(QWidget *parent, const QString &title, const QString &text, int dim);
        ~vector_dialog(void);

        static QVector2D get_vec2(QWidget *parent, const QString &title, const QString &text, const QVector2D &init = QVector2D(0., 0.), bool *ok = NULL)
        {
            vector_dialog dial(parent, title, text, 2);

            dial.spin_boxes[0].setValue(init.x());
            dial.spin_boxes[1].setValue(init.y());

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            return QVector2D(dial.spin_boxes[0].value(), dial.spin_boxes[1].value());
        }

        static QVector3D get_vec3(QWidget *parent, const QString &title, const QString &text, const QVector3D &init = QVector3D(0., 0., 0.), bool *ok = NULL)
        {
            vector_dialog dial(parent, title, text, 3);

            dial.spin_boxes[0].setValue(init.x());
            dial.spin_boxes[1].setValue(init.y());
            dial.spin_boxes[2].setValue(init.z());

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            return QVector3D(dial.spin_boxes[0].value(), dial.spin_boxes[1].value(), dial.spin_boxes[2].value());
        }

        static QVector4D get_vec4(QWidget *parent, const QString &title, const QString &text, const QVector4D &init = QVector4D(0., 0., 0., 0.), bool *ok = NULL)
        {
            vector_dialog dial(parent, title, text, 4);

            dial.spin_boxes[0].setValue(init.x());
            dial.spin_boxes[1].setValue(init.y());
            dial.spin_boxes[2].setValue(init.z());
            dial.spin_boxes[3].setValue(init.w());

            if (ok != NULL)
                *ok = dial.exec();
            else
                dial.exec();

            return QVector4D(dial.spin_boxes[0].value(), dial.spin_boxes[1].value(), dial.spin_boxes[2].value(), dial.spin_boxes[3].value());
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

#endif
