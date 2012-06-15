#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>

#include <QtCore>
#include <QtGui>

#include "application.hpp"
#include "gl_output.hpp"
#include "popup_tree.hpp"
#include "textures.hpp"
#include "uniforms.hpp"


main_window::main_window(void):
    QWidget(NULL)
{
    setWindowTitle("Shader Preview");

    QMenuBar *menu = new QMenuBar(this);

    QMenu *shader_menu = new QMenu("&Shader");

    QMenu *vsh_menu = new QMenu("&Vertex");
    QMenu *fsh_menu = new QMenu("&Fragment");

    connect(vsh_menu->addAction("&Load from file..."), SIGNAL(triggered()), this, SLOT(load_vsh()));
    connect(fsh_menu->addAction("&Load from file..."), SIGNAL(triggered()), this, SLOT(load_fsh()));

    QAction *vsh_appl_action = vsh_menu->addAction("&Apply");
    QAction *fsh_appl_action = fsh_menu->addAction("&Apply");
    connect(vsh_appl_action, SIGNAL(triggered()), this, SLOT(apply_vsh()));
    connect(fsh_appl_action, SIGNAL(triggered()), this, SLOT(apply_fsh()));
    connect(vsh_appl_action, SIGNAL(triggered()), this, SLOT(reload_uniforms()));
    connect(fsh_appl_action, SIGNAL(triggered()), this, SLOT(reload_uniforms()));

    shader_menu->addMenu(vsh_menu);
    shader_menu->addMenu(fsh_menu);

    QMenu *textures_menu = new QMenu("Te&xtures");

    connect(textures_menu->addAction("&Load..."), SIGNAL(triggered()), this, SLOT(load_tex()));
    textures_menu->addSeparator();
    connect(textures_menu->addAction("Assign to &unit..."), SIGNAL(triggered()), this, SLOT(assign_tex()));

    menu->addMenu(shader_menu);
    menu->addMenu(textures_menu);

    vsh_edit = new QPlainTextEdit(this);
    fsh_edit = new QPlainTextEdit(this);

    vsh_edit->setFont(QFont("Courier New"));
    fsh_edit->setFont(QFont("Courier New"));

    vsh_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    fsh_edit->setLineWrapMode(QPlainTextEdit::NoWrap);

    vsh_appl = new QPushButton("Apply &vertex shader");
    fsh_appl = new QPushButton("Apply &fragment shader");
    connect(vsh_appl, SIGNAL(clicked()), this, SLOT(apply_vsh()));
    connect(fsh_appl, SIGNAL(clicked()), this, SLOT(apply_fsh()));
    connect(vsh_appl, SIGNAL(clicked()), this, SLOT(reload_uniforms()));
    connect(fsh_appl, SIGNAL(clicked()), this, SLOT(reload_uniforms()));

    uniform_widget = new popup_tree_widget(this);
    uniform_widget->setColumnCount(2);
    uniform_widget->setHeaderLabels(QStringList({ "Uniform", "Value" }));

    uniform_widget->item_popup_menu = new QMenu;
    connect(uniform_widget->item_popup_menu->addAction("&Set value"), SIGNAL(triggered()), this, SLOT(set_uniform_value()));

    texture_widget = new popup_tree_widget(this);
    texture_widget->setColumnCount(2);
    texture_widget->setHeaderLabels(QStringList({ "File", "TMU" }));

    texture_widget->item_popup_menu = new QMenu;
    connect(texture_widget->item_popup_menu->addAction("Assign to &unit..."), SIGNAL(triggered()), this, SLOT(assign_tex()));

    texture_widget->bg_popup_menu = new QMenu;
    connect(texture_widget->bg_popup_menu->addAction("&Load new texture"), SIGNAL(triggered()), this, SLOT(load_tex()));

    render = new gl_output(this);

    QHBoxLayout *rnd_tex = new QHBoxLayout;

    rnd_tex->addWidget(texture_widget);
    rnd_tex->addWidget(render);

    rnd_tex->setStretchFactor(texture_widget, 0);
    rnd_tex->setStretchFactor(render, 1);

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(menu, 0, 0, 1, 3);
    layout->addWidget(vsh_edit, 1, 0);
    layout->addWidget(vsh_appl, 2, 0);
    layout->addWidget(fsh_edit, 1, 1);
    layout->addWidget(fsh_appl, 2, 1);
    layout->addWidget(uniform_widget, 1, 2, 2, 1);
    layout->addLayout(rnd_tex, 3, 0, 1, 3);

    layout->setRowStretch(1, 0);
    layout->setRowStretch(2, 0);
    layout->setRowStretch(3, 1);

    setLayout(layout);



    uniforms = new QList<uniform *>;
}

main_window::~main_window(void)
{
    delete uniforms;

    delete render;

    delete texture_widget;
    delete uniform_widget;

    delete vsh_edit;
    delete fsh_edit;
    delete vsh_appl;
    delete fsh_appl;
}


void main_window::load_vsh(void)
{
    load_sh(vsh_edit, QString("Open vertex shader source file"));
}

void main_window::load_fsh(void)
{
    load_sh(fsh_edit, QString("Open fragment shader source file"));
}


void main_window::load_tex(void)
{
    QString out = QFileDialog::getOpenFileName(this, "Open texture file");
    if (out.isEmpty())
        return;

    QFileInfo out_fi(out);

    if (!out_fi.isReadable())
    {
        QMessageBox::critical(this, "Error accessing file", "File is not readable.");
        return;
    }

    unsigned tex_id = load_texture(out, out_fi.suffix().toLower());

    if (!tex_id)
        QMessageBox::critical(this, "Error loading file", "Could not detect file format.");
    else
    {
        QTreeWidgetItem *qtwi = new QTreeWidgetItem(QStringList({ out_fi.fileName(), QString("") }));
        qtwi->setData(0, Qt::UserRole, tex_id);
        texture_widget->addTopLevelItem(qtwi);

        update_textures();
    }
}

void main_window::assign_tex(void)
{
    if (texture_widget->sel_item == NULL)
        return;

    bool ok;
    int tmu = QInputDialog::getInt(this, "Assign texture", "TMU the texture shall be assigned to?", 0, 0, render->tmus - 1, 1, &ok);

    if (ok)
    {
        for (int i = 0; i < texture_widget->topLevelItemCount(); i++)
        {
            QTreeWidgetItem *item = texture_widget->topLevelItem(i);

            bool is_mapped;
            int otmu = item->text(1).toInt(&is_mapped);

            if (!is_mapped || (otmu != tmu))
                continue;

            item->setText(1, QString(""));
        }

        texture_widget->sel_item->setText(1, QString::number(tmu));

        update_textures();
    }
}


void main_window::apply_vsh(void)
{
    render->upload_vsh(vsh_edit->toPlainText());
}

void main_window::apply_fsh(void)
{
    render->upload_fsh(fsh_edit->toPlainText());
}


void main_window::reload_uniforms(void)
{
    uniform_widget->clear();


    QString full_src = vsh_edit->toPlainText() + QString("\n") + fsh_edit->toPlainText();

    QStringList split = full_src.split(QRegExp("[;{}]"), QString::SkipEmptyParts);
    QStringList final;

    for (QString s: split)
    {
        QString done = s.trimmed();

        while (done[0] == '#')
        {
            done.remove(QRegExp("^#[^\\n]*"));
            done = done.trimmed();
        }

        if (done.isEmpty())
            continue;

        if (!done.contains(QRegExp("^uniform\\s+")))
            continue;

        final.push_back(done.remove(QRegExp("^uniform\\s+")));
    }


    QList<uniform *> *new_uniforms = new QList<uniform *>;


    for (QString s: final)
    {
        QRegExp regex("^([^\\s]+)\\s+(.+)$");

        if (regex.indexIn(s) < 0)
            continue;

        QString type = regex.cap(1);

        for (QString identifier: regex.cap(2).split(QRegExp("\\s*,\\s*"), QString::SkipEmptyParts))
        {
            uniform *nu = NULL;

            for (uniform *u: *uniforms)
            {
                if ((u->name == identifier) && (u->type == type))
                {
                    nu = u;
                    // There can be only one, anyway, thus this should be faster
                    uniforms->removeOne(u);
                    break;
                }
            }

            if (nu == NULL)
                nu = new uniform(type, identifier);

            new_uniforms->push_back(nu);
        }
    }


    for (uniform *u: *uniforms)
        delete u;

    delete uniforms;
    uniforms = new_uniforms;


    for (uniform *u: *uniforms)
    {
        u->id = 0;
        render->update_uniform(u);

        QTreeWidgetItem *qtwi = new QTreeWidgetItem(uniform_widget, QStringList({ u->name, u->value }));
        qtwi->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void *>(u)));
        uniform_widget->addTopLevelItem(qtwi);
    }
}

void main_window::set_uniform_value(void)
{
    if (uniform_widget->sel_item == NULL)
        return;

    uniform *u = static_cast<uniform *>(uniform_widget->sel_item->data(0, Qt::UserRole).value<void *>());

    if (u->type == "sampler2D")
    {
        QStringList textures;

        for (int i = 0; i < texture_widget->topLevelItemCount(); i++)
        {
            QTreeWidgetItem *item = texture_widget->topLevelItem(i);

            bool is_mapped;
            int tmu = item->text(1).toInt(&is_mapped);

            if (!is_mapped)
                continue;

            textures.push_back(QString::number(tmu) + ": " + item->text(0));
        }

        if (textures.isEmpty())
        {
            QMessageBox::critical(this, "Change uniform value", "No texture assigned to any TMU.");
            return;
        }

        bool ok;
        QString chosen = QInputDialog::getItem(this, "Change uniform value", "New texture to be assigned to this sampler:", textures, 0, false, &ok);

        if (ok)
        {
            *u = QVariant(chosen.remove(QRegExp(":.*$")).toInt());

            render->update_uniform(u);

            uniform_widget->sel_item->setText(1, u->value);
        }
    }
    else if (u->proc_type == uniform::t_float)
    {
        bool ok;
        float val = QInputDialog::getDouble(this, "Change uniform value", "New floating point value to be assigned to this uniform:", 0., -HUGE_VAL, HUGE_VAL, 10, &ok);

        if (ok)
        {
            *u = QVariant(val);

            render->update_uniform(u);

            uniform_widget->sel_item->setText(1, u->value);
        }
    }
    else
        QMessageBox::critical(this, "Change uniform value", "Unknown type “" + u->type + "” of uniform “" + u->name + "”.");
}


void main_window::load_sh(QPlainTextEdit *edit, const QString &sel_title)
{
    QString out = QFileDialog::getOpenFileName(this, sel_title);
    if (out.isEmpty())
        return;

    QByteArray out_arr = out.toUtf8();

    FILE *fp = fopen(out_arr.constData(), "r");
    if (fp == NULL)
    {
        QMessageBox::critical(this, "Error loading file", QString("Could not open the given file: ") + strerror(errno));
        return;
    }

    fseek(fp, 0, SEEK_END);
    long lof = ftell(fp);
    rewind(fp);

    char *buf = new char[lof + 1];
    long bread = fread(buf, 1, lof, fp);
    fclose(fp);

    if (bread < lof)
    {
        delete buf;
        QMessageBox::critical(this, "Error loading file", QString("An error occured while reading the given file: ") + strerror(errno));
        return;
    }

    buf[lof] = 0;

    edit->setPlainText(QString(buf));

    delete buf;
}

void main_window::update_textures(void)
{
    for (int i = 0; i < texture_widget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = texture_widget->topLevelItem(i);

        bool is_mapped;
        int tmu = item->text(1).toInt(&is_mapped);

        if (!is_mapped)
            continue;

        render->bind_tex_to(item->data(0, Qt::UserRole).value<unsigned>(), tmu);
    }
}
