#include <QtGui>

#include "dialogs.hpp"
#include "texture_management.hpp"
#include "trackable.hpp"
#include "uniform.hpp"


vector_dialog::vector_dialog(QWidget *parent, const QString &title, const QString &text, int dim):
    QDialog(parent)
{
    dimension = dim;


    setWindowTitle(title);

    text_label = new QLabel(text, this);
    spin_boxes = new QDoubleSpinBox[dim];

    for (int i = 0; i < dim; i++)
    {
        spin_boxes[i].setDecimals(10);
        spin_boxes[i].setRange(-HUGE_VAL, HUGE_VAL);
    }

    color_button = ((dim == 3) || (dim == 4)) ? (new QPushButton("&Choose color", this)) : NULL;

    accept_button = new QPushButton("&OK", this);
    reject_button = new QPushButton("&Cancel", this);


    coord_layout = new QHBoxLayout;
    for (int i = 0; i < dim; i++)
        coord_layout->addWidget(&spin_boxes[i]);

    button_layout = new QHBoxLayout;
    button_layout->addWidget(color_button);
    button_layout->addStretch(1);
    button_layout->addWidget(accept_button);
    button_layout->addWidget(reject_button);

    top_layout = new QVBoxLayout;
    top_layout->addWidget(text_label);
    top_layout->addLayout(coord_layout);
    top_layout->addStretch(1);
    top_layout->addLayout(button_layout);

    setLayout(top_layout);


    if ((dim == 3) || (dim == 4))
        connect(color_button, SIGNAL(clicked()), this, SLOT(get_color()));

    connect(accept_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(reject_button, SIGNAL(clicked()), this, SLOT(reject()));
}

vector_dialog::~vector_dialog(void)
{
    delete text_label;
    delete[] spin_boxes;
    delete color_button;
    delete accept_button;
    delete reject_button;

    delete coord_layout;
    delete button_layout;
    delete top_layout;
}


void vector_dialog::get_color(void)
{
    QColor col;

    if (dimension == 3)
        col = QColorDialog::getColor(QColor::fromRgbF(spin_boxes[0].value(), spin_boxes[1].value(), spin_boxes[2].value()), this, "Select color");
    else if (dimension == 4)
        col = QColorDialog::getColor(QColor::fromRgbF(spin_boxes[0].value(), spin_boxes[1].value(), spin_boxes[2].value(), spin_boxes[3].value()), this, "Select color", QColorDialog::ShowAlphaChannel);

    if (col.isValid())
    {
        spin_boxes[0].setValue(col.redF());
        spin_boxes[1].setValue(col.greenF());
        spin_boxes[2].setValue(col.blueF());

        if (dimension == 4)
            spin_boxes[3].setValue(col.alphaF());
    }
}


texture_dialog::texture_dialog(QWidget *parent, const QString &title, const QString &text):
    QDialog(parent)
{
    setWindowTitle(title);

    text_label = new QLabel(text, this);
    choose_box = new QComboBox(this);

    for (managed_texture *mt: textures)
        choose_box->addItem(mt->name, QVariant::fromValue(static_cast<void *>(mt)));

    load_button = new QPushButton("&Load from file", this);

    accept_button = new QPushButton("&OK", this);
    reject_button = new QPushButton("&Cancel", this);


    button_layout = new QHBoxLayout;
    button_layout->addWidget(load_button);
    button_layout->addStretch(1);
    button_layout->addWidget(accept_button);
    button_layout->addWidget(reject_button);

    top_layout = new QVBoxLayout;
    top_layout->addWidget(text_label);
    top_layout->addWidget(choose_box);
    top_layout->addStretch(1);
    top_layout->addLayout(button_layout);

    setLayout(top_layout);


    connect(load_button, SIGNAL(clicked()), this, SLOT(load_tex()));

    connect(accept_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(reject_button, SIGNAL(clicked()), this, SLOT(reject()));
}

texture_dialog::~texture_dialog(void)
{
    delete text_label;
    delete choose_box;

    delete load_button;
    delete accept_button;
    delete reject_button;

    delete button_layout;
    delete top_layout;
}

void texture_dialog::load_tex(void)
{
    QString load = QFileDialog::getOpenFileName(this, "Open texture file");
    if (load.isEmpty())
        return;

    managed_texture *mt = load_texture(load);
    if (mt == NULL)
    {
        QMessageBox::critical(this, "Error loading file", "The given file could not be loaded as a texture."); // TODO: Be more specific
        return;
    }

    choose_box->addItem(mt->name, QVariant::fromValue(static_cast<void *>(mt)));
    choose_box->setCurrentIndex(choose_box->count() - 1);
}


trackable_dialog::trackable_dialog(QWidget *parent, const QString &title, const QString &text, uniform::utype type):
    QDialog(parent)
{
    setWindowTitle(title);

    text_label = new QLabel(text, this);
    choose_box = new QComboBox(this);

    for (trackable *t: trackables)
        if (t->type == type)
            choose_box->addItem(t->name, QVariant::fromValue(static_cast<void *>(t)));

    accept_button = new QPushButton("&OK", this);
    reject_button = new QPushButton("&Cancel", this);


    button_layout = new QHBoxLayout;
    button_layout->addStretch(1);
    button_layout->addWidget(accept_button);
    button_layout->addWidget(reject_button);

    top_layout = new QVBoxLayout;
    top_layout->addWidget(text_label);
    top_layout->addWidget(choose_box);
    top_layout->addStretch(1);
    top_layout->addLayout(button_layout);

    setLayout(top_layout);


    connect(accept_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(reject_button, SIGNAL(clicked()), this, SLOT(reject()));
}

trackable_dialog::~trackable_dialog(void)
{
    delete text_label;
    delete choose_box;

    delete accept_button;
    delete reject_button;

    delete button_layout;
    delete top_layout;
}
