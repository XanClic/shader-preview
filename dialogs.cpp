#include <QtGui>

#include "dialogs.hpp"
#include "texture_management.hpp"
#include "trackable.hpp"
#include "uniform.hpp"
#include "vertex_info.hpp"


vector_dialog::vector_dialog(QWidget *rparent, const QString &title, const QString &text, int dim):
    QDialog(rparent)
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
    if (color_button != NULL)
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


matrix_dialog::matrix_dialog(QWidget *rparent, const QString &title, const QString &text, int dim):
    QDialog(rparent)
{
    dimension = dim;


    setWindowTitle(title);

    text_label = new QLabel(text, this);
    spin_boxes = new QDoubleSpinBox[dim * dim];

    for (int i = 0; i < dim * dim; i++)
    {
        spin_boxes[i].setDecimals(10);
        spin_boxes[i].setRange(-HUGE_VAL, HUGE_VAL);
    }

    accept_button = new QPushButton("&OK", this);
    reject_button = new QPushButton("&Cancel", this);


    if (dim == 4)
    {
        identity_button = new QPushButton("&Identity", this);
        persp_button = new QPushButton("&Perspective", this);
        ortho_button = new QPushButton("&Orthographic", this);
        trans_button = new QPushButton("&Translate", this);
        scale_button = new QPushButton("&Scale", this);
        rotate_button = new QPushButton("&Rotate", this);

        connect(identity_button, SIGNAL(clicked()), this, SLOT(load_identity()));
        connect(persp_button, SIGNAL(clicked()), this, SLOT(load_perspective()));
        connect(ortho_button, SIGNAL(clicked()), this, SLOT(load_orthographic()));
        connect(trans_button, SIGNAL(clicked()), this, SLOT(translate()));
        connect(scale_button, SIGNAL(clicked()), this, SLOT(scale()));
        connect(rotate_button, SIGNAL(clicked()), this, SLOT(rotate()));
    }


    coord_layouts = new QHBoxLayout[dim];
    for (int j = 0; j < dim; j++)
        for (int i = 0; i < dim; i++)
            coord_layouts[j].addWidget(&spin_boxes[j + i * 4]);

    if (dim == 4)
    {
        projection_layout = new QHBoxLayout;
        projection_layout->addWidget(identity_button);
        projection_layout->addWidget(persp_button);
        projection_layout->addWidget(ortho_button);
        transform_layout = new QHBoxLayout;
        transform_layout->addWidget(trans_button);
        transform_layout->addWidget(scale_button);
        transform_layout->addWidget(rotate_button);
    }

    button_layout = new QHBoxLayout;
    button_layout->addStretch(1);
    button_layout->addWidget(accept_button);
    button_layout->addWidget(reject_button);

    top_layout = new QVBoxLayout;
    top_layout->addWidget(text_label);
    for (int i = 0; i < dim; i++)
        top_layout->addLayout(&coord_layouts[i]);

    if (dim == 4)
    {
        top_layout->addLayout(projection_layout);
        top_layout->addLayout(transform_layout);
    }

    top_layout->addStretch(1);
    top_layout->addLayout(button_layout);

    setLayout(top_layout);


    connect(accept_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(reject_button, SIGNAL(clicked()), this, SLOT(reject()));
}

matrix_dialog::~matrix_dialog(void)
{
    delete text_label;
    delete[] spin_boxes;
    delete accept_button;
    delete reject_button;
    if (dimension == 4)
    {
        delete identity_button;
        delete persp_button;
        delete ortho_button;
        delete trans_button;
        delete scale_button;
        delete rotate_button;
    }

    delete[] coord_layouts;
    if (dimension == 4)
    {
        delete projection_layout;
        delete transform_layout;
    }
    delete button_layout;
    delete top_layout;
}


void matrix_dialog::set_mat4(const mat4 &d)
{
    if (dimension != 4)
        return;

    for (int i = 0; i < 16; i++)
        spin_boxes[i].setValue(d.d[i]);
}

mat4 matrix_dialog::get_mat4(void)
{
    if (dimension != 4)
        return mat4();

    mat4 m;
    for (int i = 0; i < 16; i++)
        m.d[i] = spin_boxes[i].value();

    return m;
}


void matrix_dialog::load_identity(void)
{
    set_mat4(mat4());
}

void matrix_dialog::load_perspective(void)
{
    bool ok = false;

    vec4 paras = vector_dialog::get_vec4(this, "Perspective projection", "Aspect (width to height), FOV (horizontal, degrees), near plane, far plane:", vec4(1., 60., .1f, 100.f), &ok);

    if (!ok)
        return;

    mat4 persp;

    float f = 1.f / tanf(paras[1] * (float)M_PI / 360.f);
    persp.d[ 0] = f * paras[0];
    persp.d[ 5] = f;
    persp.d[10] = (      paras[2] + paras[3]) / (paras[2] - paras[3]);
    persp.d[11] = -1.f;
    persp.d[14] = (2.f * paras[2] * paras[3]) / (paras[2] - paras[3]);
    persp.d[15] = 0.f;

    set_mat4(persp * get_mat4());
}

void matrix_dialog::load_orthographic(void)
{
    // FIXME: user-defined z planes

    bool ok = false;

    vec4 paras = vector_dialog::get_vec4(this, "Orthographic projection", "Clipping planes (left, right, bottom, top):", vec4(-1.f, 1.f, -1.f, 1.f), &ok);

    if (!ok)
        return;

    mat4 ortho;

    ortho.d[ 0] = 2.f / (paras[1] - paras[0]);
    ortho.d[ 5] = 2.f / (paras[3] - paras[2]);
    ortho.d[10] = -1.f; // far plane @1, near plane @-1
    ortho.d[12] = (paras[0] + paras[1]) / (paras[0] - paras[1]);
    ortho.d[13] = (paras[2] + paras[3]) / (paras[2] - paras[3]);
    ortho.d[14] = 0.f;

    set_mat4(ortho * get_mat4());
}

void matrix_dialog::translate(void)
{
    bool ok = false;

    vec3 vec = vector_dialog::get_vec3(this, "Translation", "Translate matrix by:", vec3(0.f, 0.f, 0.f), &ok);

    if (!ok)
        return;

    mat4 m(get_mat4());
    set_mat4(m.translate(vec));
}

void matrix_dialog::scale(void)
{
    bool ok = false;

    vec3 vec = vector_dialog::get_vec3(this, "Scale", "Scale matrix by:", vec3(1.f, 1.f, 1.f), &ok);

    if (!ok)
        return;

    mat4 m(get_mat4());
    set_mat4(m.scale(vec));
}

void matrix_dialog::rotate(void)
{
    bool ok = false;

    vec4 vec = vector_dialog::get_vec4(this, "Rotation", "Rotate [field 1] degrees around vector [fields 2 to 4]:", vec4(0.f, 0.f, 1.f, 0.f), &ok);

    if (!ok)
        return;

    mat4 m(get_mat4());
    set_mat4(m.rotate(vec[0] * (float)M_PI / 180.f, vec3(vec[1], vec[2], vec[3])));
}


texture_dialog::texture_dialog(QWidget *rparent, const QString &title, const QString &text):
    QDialog(rparent)
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


trackable_dialog::trackable_dialog(QWidget *rparent, const QString &title, const QString &text, uniform::utype type):
    QDialog(rparent)
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


wavefront_load_dialog::wavefront_load_dialog(QWidget *rparent, const QString &title, const QString &text, const QList<vertex_attrib *> &vas):
    QDialog(rparent)
{
    setWindowTitle(title);


    ignore_asgn = false;

    assignments = new QList<wavefront_assignment *>;


    text_label = new QLabel(text, this);
    va_box = new QComboBox(this);

    for (vertex_attrib *va: vas)
    {
        wavefront_assignment *wa;

        switch (va->epv)
        {
            case 1: wa = new wavefront_fixed_assignment_float; static_cast<wavefront_fixed_assignment_float *>(wa)->val = 0.f; break;
            case 2: wa = new wavefront_fixed_assignment_vec2;  static_cast<wavefront_fixed_assignment_vec2  *>(wa)->val = vec2(0.f, 0.f); break;
            case 3: wa = new wavefront_fixed_assignment_vec3;  static_cast<wavefront_fixed_assignment_vec3  *>(wa)->val = vec3(0.f, 0.f, 0.f); break;
            case 4: wa = new wavefront_fixed_assignment_vec4;  static_cast<wavefront_fixed_assignment_vec4  *>(wa)->val = vec4(0.f, 0.f, 0.f, 0.f); break;
        }

        wa->va = va;
        wa->fixed = true;

        assignments->push_back(wa);
        va_box->addItem(va->name, QVariant::fromValue(static_cast<void *>(wa)));
    }

    va_box->setCurrentIndex(0);

    connect(va_box, SIGNAL(currentIndexChanged(int)), this, SLOT(va_changed(int)));


    asgn_box = new QComboBox(this);

    asgn_box->addItem("Fixed value", 0);
    asgn_box->addItem("Vertex coordinates", 1);
    asgn_box->addItem("Texture coordinates", 2);
    asgn_box->addItem("Vertex normals", 3);

    asgn_box->setCurrentIndex(0);

    connect(asgn_box, SIGNAL(currentIndexChanged(int)), this, SLOT(asgn_changed(int)));


    spin_boxes = new QDoubleSpinBox[4];
    for (int i = 0; i < 4; i++)
    {
        spin_boxes[i].setDecimals(10);
        spin_boxes[i].setRange(-HUGE_VAL, HUGE_VAL);

        if (i >= (*assignments)[0]->va->epv)
            spin_boxes[i].setEnabled(false);

        connect(&spin_boxes[i], SIGNAL(valueChanged(double)), this, SLOT(val_changed(double)));
    }


    accept_button = new QPushButton("&OK", this);
    reject_button = new QPushButton("&Cancel", this);


    coord_layout = new QHBoxLayout;
    for (int i = 0; i < 4; i++)
        coord_layout->addWidget(&spin_boxes[i]);

    button_layout = new QHBoxLayout;
    button_layout->addStretch(1);
    button_layout->addWidget(accept_button);
    button_layout->addWidget(reject_button);

    top_layout = new QVBoxLayout;
    top_layout->addWidget(text_label);
    top_layout->addWidget(va_box);
    top_layout->addWidget(asgn_box);
    top_layout->addLayout(coord_layout);
    top_layout->addStretch(1);
    top_layout->addLayout(button_layout);


    setLayout(top_layout);


    connect(accept_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(reject_button, SIGNAL(clicked()), this, SLOT(reject()));
}

wavefront_load_dialog::~wavefront_load_dialog(void)
{
    delete text_label;
    delete va_box;
    delete asgn_box;
    delete[] spin_boxes;

    delete accept_button;
    delete reject_button;

    delete button_layout;
    delete coord_layout;
    delete top_layout;
}


void wavefront_load_dialog::va_changed(int ni)
{
    wavefront_assignment *wa = static_cast<wavefront_assignment *>(va_box->itemData(ni).value<void *>());

    if (wa->fixed)
    {
        ignore_asgn = true; // FIXME
        asgn_box->setCurrentIndex(0);
        ignore_asgn = false;

        float vals[4] = { 0.f, 0.f, 0.f, 0.f };

        switch (wa->va->epv)
        {
            case 1:
            {
                wavefront_fixed_assignment_float *wa1 = static_cast<wavefront_fixed_assignment_float *>(wa);
                vals[0] = wa1->val;
                break;
            }
            case 2:
            {
                wavefront_fixed_assignment_vec2 *wa2 = static_cast<wavefront_fixed_assignment_vec2 *>(wa);
                vals[0] = wa2->val.x;
                vals[1] = wa2->val.y;
                break;
            }
            case 3:
            {
                wavefront_fixed_assignment_vec3 *wa3 = static_cast<wavefront_fixed_assignment_vec3 *>(wa);
                vals[0] = wa3->val.x;
                vals[1] = wa3->val.y;
                vals[2] = wa3->val.z;
                break;
            }
            case 4:
            {
                wavefront_fixed_assignment_vec4 *wa4 = static_cast<wavefront_fixed_assignment_vec4 *>(wa);
                vals[0] = wa4->val.x;
                vals[1] = wa4->val.y;
                vals[2] = wa4->val.z;
                vals[3] = wa4->val.w;
                break;
            }
        }

        for (int i = 0; i < 4; i++)
        {
            spin_boxes[i].setValue(vals[i]);
            spin_boxes[i].setEnabled(i < wa->va->epv);
        }
    }
    else
    {
        ignore_asgn = true;
        asgn_box->setCurrentIndex(static_cast<wavefront_var_assignment *>(wa)->val + 1);
        ignore_asgn = false;

        for (int i = 0; i < 4; i++)
        {
            spin_boxes[i].setValue(0.f);
            spin_boxes[i].setEnabled(false);
        }
    }
}


void wavefront_load_dialog::asgn_changed(int ni)
{
    if (ignore_asgn)
        return;


    wavefront_assignment *wa = static_cast<wavefront_assignment *>(va_box->itemData(va_box->currentIndex()).value<void *>()), *nwa;


    if (!ni)
    {
        // TODO: Keep values (i.e., don't reset to zero if this was selected as fixed before)

        switch (wa->va->epv)
        {
            case 1: nwa = new wavefront_fixed_assignment_float; static_cast<wavefront_fixed_assignment_float *>(nwa)->val = 0.f; break;
            case 2: nwa = new wavefront_fixed_assignment_vec2;  static_cast<wavefront_fixed_assignment_vec2  *>(nwa)->val = vec2(0.f, 0.f); break;
            case 3: nwa = new wavefront_fixed_assignment_vec3;  static_cast<wavefront_fixed_assignment_vec3  *>(nwa)->val = vec3(0.f, 0.f, 0.f); break;
            case 4: nwa = new wavefront_fixed_assignment_vec4;  static_cast<wavefront_fixed_assignment_vec4  *>(nwa)->val = vec4(0.f, 0.f, 0.f, 0.f); break;
        }

        nwa->va = wa->va;
        nwa->fixed = true;

        for (int i = 0; i < 4; i++)
        {
            spin_boxes[i].setValue(0.f);
            spin_boxes[i].setEnabled(i < nwa->va->epv);
        }
    }
    else
    {
        nwa = new wavefront_var_assignment;

        nwa->va = wa->va;
        nwa->fixed = false;

        static_cast<wavefront_var_assignment *>(nwa)->val = static_cast<wavefront_var_assignment::list>(ni - 1);

        for (int i = 0; i < 4; i++)
        {
            spin_boxes[i].setValue(0.f);
            spin_boxes[i].setEnabled(false);
        }
    }


    assignments->removeAll(wa);
    delete wa;

    assignments->push_back(nwa);


    va_box->setItemData(va_box->currentIndex(), QVariant::fromValue(static_cast<void *>(nwa)));
}


void wavefront_load_dialog::val_changed(double d)
{
    (void)d;

    wavefront_assignment *wa = static_cast<wavefront_assignment *>(va_box->itemData(va_box->currentIndex()).value<void *>());

    switch (wa->va->epv)
    {
        case 1: static_cast<wavefront_fixed_assignment_float *>(wa)->val = spin_boxes[0].value(); break;
        case 2: static_cast<wavefront_fixed_assignment_vec2  *>(wa)->val = vec2(spin_boxes[0].value(), spin_boxes[1].value()); break;
        case 3: static_cast<wavefront_fixed_assignment_vec3  *>(wa)->val = vec3(spin_boxes[0].value(), spin_boxes[1].value(), spin_boxes[2].value()); break;
        case 4: static_cast<wavefront_fixed_assignment_vec4  *>(wa)->val = vec4(spin_boxes[0].value(), spin_boxes[1].value(), spin_boxes[2].value(), spin_boxes[3].value()); break;
    }
}
