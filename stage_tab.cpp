#define GL_GLEXT_PROTOTYPES

#include <cstdio>

#include <QtGui>

#include "color_buffer.hpp"
#include "dialogs.hpp"
#include "opengl.hpp"
#include "popup_tree.hpp"
#include "renderer.hpp"
#include "stage_tab.hpp"
#include "uniform.hpp"
#include "vertex_info.hpp"


stage_tab::stage_tab(int sn, renderer *rdr, QWidget *parent):
    QWidget(parent),
    vsh_edit(this), gsh_edit(this), fsh_edit(this),
    sh_apply("Appl&y shaders", this),
    uniform_tree(this), vertex_tree(this), buffer_tree(this),
    gsh_enable("Enable &geometry shader"), vtx_type(this),
    vertex_rendering_method(GL_QUADS), stage_number(sn),
    render(rdr),
    ready_to_roll(false)
{
    vsh_edit.setFont(QFont("Courier New"));
    gsh_edit.setFont(QFont("Courier New"));
    fsh_edit.setFont(QFont("Courier New"));

    vsh_edit.setLineWrapMode(QPlainTextEdit::NoWrap);
    gsh_edit.setLineWrapMode(QPlainTextEdit::NoWrap);
    fsh_edit.setLineWrapMode(QPlainTextEdit::NoWrap);


    connect(&sh_apply, SIGNAL(clicked()), this, SLOT(apply_shaders()));
    connect(&gsh_enable, SIGNAL(stateChanged(int)), this, SLOT(enable_gsh(int)));


    uniform_tree.setColumnCount(2);
    uniform_tree.setHeaderLabels(QStringList({ "Uniform", "Value" }));

    uniform_tree.item_popup_menu = new QMenu;
    connect(uniform_tree.item_popup_menu->addAction("&Set value"), SIGNAL(triggered()), this, SLOT(assign_uniform()));
    connect(uniform_tree.item_popup_menu->addAction("Track &builtin"), SIGNAL(triggered()), this, SLOT(bind_builtin()));


    vertex_tree.setColumnCount(2);
    vertex_tree.setHeaderLabels(QStringList({ "Attribute", "Value" }));

    vertex_tree.item_popup_menu = new QMenu;
    connect(vertex_tree.item_popup_menu->addAction("&Set attribute value"), SIGNAL(triggered()), this, SLOT(set_vertex_attribute()));
    vertex_tree.item_popup_menu->addSeparator();
    connect(vertex_tree.item_popup_menu->addAction("&Add vertex"), SIGNAL(triggered()), this, SLOT(add_vertex()));

    vertex_tree.bg_popup_menu = new QMenu;
    connect(vertex_tree.bg_popup_menu->addAction("&Add vertex"), SIGNAL(triggered()), this, SLOT(add_vertex()));


    buffer_tree.setColumnCount(1);
    buffer_tree.setHeaderLabels(QStringList({ "Color buffer" }));


    vtx_type.addItem("Points", (int)GL_POINTS);
    vtx_type.addItem("Lines", (int)GL_LINES);
    vtx_type.addItem("Line strip", (int)GL_LINE_STRIP);
    vtx_type.addItem("Line loop", (int)GL_LINE_LOOP);
    vtx_type.addItem("Triangles", (int)GL_TRIANGLES);
    vtx_type.addItem("Triangle strip", (int)GL_TRIANGLE_STRIP);
    vtx_type.addItem("Triangle fan", (int)GL_TRIANGLE_FAN);
    vtx_type.addItem("Quads", (int)GL_QUADS);
    vtx_type.addItem("Quad strip", (int)GL_QUAD_STRIP);
    vtx_type.setCurrentIndex(7);

    connect(&vtx_type, SIGNAL(currentIndexChanged(int)), this, SLOT(different_vertex_rendering_method(int)));



    gsh_layout.addWidget(&gsh_edit);
    gsh_layout.addWidget(&gsh_enable);

    sub_layout[0].addWidget(&vsh_edit);
    sub_layout[0].addLayout(&gsh_layout);
    sub_layout[0].addWidget(&fsh_edit);

    vtx_layout.addWidget(&vertex_tree);
    vtx_layout.addWidget(&vtx_type);

    sub_layout[1].addLayout(&vtx_layout);
    sub_layout[1].addWidget(&buffer_tree);

    main_layout.addLayout(&sub_layout[0]);
    main_layout.addWidget(&sh_apply);
    main_layout.addWidget(&uniform_tree);
    main_layout.addLayout(&sub_layout[1]);

    setLayout(&main_layout);


    uniforms = new QList<uniform *>;
    vertices = new vertex_info;
    outputs = new QList<color_buffer *>;


    gsh_edit.setEnabled(false);
    if (ogl_maj < 3)
        gsh_enable.setEnabled(false);


    if (ogl_maj >= 3)
    {
        vsh_edit.setPlainText("#version 130\n\nin vec4 vertex;\n\nuniform mat4 projection, modelview;\n\nvoid main(void)\n{\n    gl_Position = projection * modelview * vertex;\n}");
        fsh_edit.setPlainText("#version 130\n\nout vec4 color;\n\nvoid main(void)\n{\n    color = vec4(1.0, 0.25, 0.0, 1.0);\n}");
    }
    else
    {
        vsh_edit.setPlainText("#version 110\n\nuniform mat4 projection, modelview;\n\nvoid main(void)\n{\n    gl_Position = projection * modelview * gl_Vertex;\n}");
        fsh_edit.setPlainText("#version 110\n\nvoid main(void)\n{\n    gl_FragColor = vec4(1.0, 0.25, 0.0, 1.0);\n}");
    }

    if (!rpd.update_shaders(this)) throw 42; // FIXME

    scan_shaders();

    vertex_attrib_vec4 *va = new vertex_attrib_vec4((ogl_maj >= 3) ? "vertex" : "gl_Vertex");
    va->values.push_back(vec4(-1.f,  1.f, 0.f, 1.f));
    va->values.push_back(vec4(-1.f, -1.f, 0.f, 1.f));
    va->values.push_back(vec4( 1.f, -1.f, 0.f, 1.f));
    va->values.push_back(vec4( 1.f,  1.f, 0.f, 1.f));
    vertices->attribs.push_back(va);

    vertices->update_buffer();

    (*uniforms)[0]->track = render->projection;
    (*uniforms)[1]->track = render->modelview;

    (*uniforms)[0]->valstr = "Built-in: Projection matrix";
    (*uniforms)[1]->valstr = "Built-in: Modelview matrix";

    scan_shaders();

    rpd.update_fbo(this);
    rpd.update_uniforms(this);
    rpd.update_vertex_buffers(this);


    ready_to_roll = true;
}

stage_tab::~stage_tab(void)
{
    for (uniform *u: *uniforms)
    {
        if (u->type == uniform::t_sampler2d)
            unuse_texture(static_cast<sampler2d_uniform *>(u)->val);

        delete u;
    }

    delete uniforms;


    for (color_buffer *cb: *outputs)
        delete cb;

    delete outputs;


    delete vertices;
}


static uniform::utype get_type_from_string(const QString &str)
{
    if (str == "int")
        return uniform::t_int;
    else if (str == "sampler2D")
        return uniform::t_sampler2d;
    else if (str == "float")
        return uniform::t_float;
    else if (str == "vec2")
        return uniform::t_vec2;
    else if (str == "vec3")
        return uniform::t_vec3;
    else if (str == "vec4")
        return uniform::t_vec4;
    else if (str == "mat2")
        return uniform::t_mat2;
    else if (str == "mat3")
        return uniform::t_mat3;
    else if (str == "mat4")
        return uniform::t_mat4;
    else
        return uniform::t_unknown;
}

void stage_tab::scan_shaders(void)
{
    uniform_tree.clear();
    vertex_tree.clear();
    buffer_tree.clear();


    QString vsh_src = vsh_edit.toPlainText();
    QString gsh_src = gsh_edit.isEnabled() ? gsh_edit.toPlainText() : "";
    QString fsh_src = fsh_edit.toPlainText();

    if (ogl_maj < 3)
    {
        vsh_src += "\nin vec4 gl_Vertex, gl_MultiTexCoord0, gl_Color, gl_Normal;";
        // TODO: gl_FragData[i]
        fsh_src += "\nout vec4 gl_FragColor;";
    }

    QStringList vsh_split = vsh_src.split(QRegExp("[;{}]"), QString::SkipEmptyParts);
    QStringList gsh_split = gsh_src.split(QRegExp("[;{}]"), QString::SkipEmptyParts);
    QStringList fsh_split = fsh_src.split(QRegExp("[;{}]"), QString::SkipEmptyParts);

    QStringList uniform_defs, vbo_defs, output_defs;

    QStringList *splits[3] = { &vsh_split, &gsh_split, &fsh_split };

    for (QStringList *split: splits)
    {
        for (QString s: *split)
        {
            QString done = s.trimmed();

            while (done[0] == '#')
            {
                done.remove(QRegExp("^#[^\\n]*"));
                done = done.trimmed();
            }

            if (done.isEmpty())
                continue;

            if (done.contains(QRegExp("^uniform\\s+")))
                uniform_defs.push_back(done.remove(QRegExp("^uniform\\s+")));
            else if ((split == &vsh_split) && done.contains(QRegExp("^in\\s+")))
                vbo_defs.push_back(done.remove(QRegExp("^in\\s+")));
            else if ((split == &fsh_split) && done.contains(QRegExp("^out\\s+")))
                output_defs.push_back(done.remove(QRegExp("^out\\s+")));
        }
    }


    QList<uniform *> *new_uniforms = new QList<uniform *>;
    QList<color_buffer *> *new_outputs = new QList<color_buffer *>;

    QStringList *vars[3] = { &uniform_defs, &vbo_defs, &output_defs };

    for (QStringList *var: vars)
    {
        for (QString s: *var)
        {
            QRegExp regex("^([^\\s]+)\\s+(.+)$");

            if (regex.indexIn(s) < 0)
                continue;

            QString type = regex.cap(1);

            for (QString identifier: regex.cap(2).split(QRegExp("\\s*,\\s*"), QString::SkipEmptyParts))
            {
                if (var == &uniform_defs)
                {
                    uniform *nu = NULL;

                    for (uniform *u: *uniforms)
                    {
                        if ((u->name == identifier) && (u->type == get_type_from_string(type)))
                        {
                            nu = u;
                            // There can be only one, anyway, thus this should be faster
                            uniforms->removeOne(u);
                            break;
                        }
                    }

                    if (nu == NULL)
                    {
                        if (type == "float")
                            nu = new float_uniform;
                        else if (type == "int")
                            nu = new int_uniform;
                        else if (type == "sampler2D")
                            nu = new sampler2d_uniform;
                        else if (type == "vec2")
                            nu = new vec2_uniform;
                        else if (type == "vec3")
                            nu = new vec3_uniform;
                        else if (type == "vec4")
                            nu = new vec4_uniform;
                        else if (type == "mat2")
                            nu = new mat2_uniform;
                        else if (type == "mat3")
                            nu = new mat3_uniform;
                        else if (type == "mat4")
                            nu = new mat4_uniform;
                        else
                            throw 42; // FIXME

                        nu->name = identifier;
                    }

                    new_uniforms->push_back(nu);
                }
                else if (var == &output_defs)
                {
                    color_buffer *ncb = NULL;

                    for (color_buffer *cb: *outputs)
                    {
                        if (cb->name == identifier)
                        {
                            ncb = cb;
                            outputs->removeOne(cb);
                            break;
                        }
                    }

                    if (ncb == NULL)
                        ncb = new color_buffer("Stage " + QString::number(stage_number), identifier);

                    new_outputs->push_back(ncb);
                }
            }
        }
    }


    for (uniform *u: *uniforms)
    {
        if (u->type == uniform::t_sampler2d)
            unuse_texture(static_cast<sampler2d_uniform *>(u)->val);

        delete u;
    }

    delete uniforms;

    uniforms = new_uniforms;


    for (color_buffer *cb: *outputs)
    {
        if (cb->id == render->tex_bound->id)
            render->tex_bound = NULL;

        delete cb;
    }

    delete outputs;

    outputs = new_outputs;


    for (uniform *u: *uniforms)
    {
        QTreeWidgetItem *qtwi = new QTreeWidgetItem(&uniform_tree, QStringList({ u->name, u->valstr }));
        qtwi->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void *>(u)));
        uniform_tree.addTopLevelItem(qtwi);
    }

    for (color_buffer *cb: *outputs)
    {

        QTreeWidgetItem *qtwi = new QTreeWidgetItem(&buffer_tree, QStringList(cb->name));
        qtwi->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void *>(cb)));
        buffer_tree.addTopLevelItem(qtwi);
    }

    if (vertices->attribs.length() < 1)
        return;

    int vertex_count = vertices->attribs[0]->len();
    for (int v = 0; v < vertex_count; v++)
    {
        QTreeWidgetItem *qtwi = new QTreeWidgetItem(&vertex_tree, QStringList(QString::number(v)));
        vertex_tree.addTopLevelItem(qtwi);

        for (vertex_attrib *va: vertices->attribs)
        {
            QString valstr;

            switch (va->epv)
            {
                case 1:
                    valstr = QString::number(static_cast<vertex_attrib_float *>(va)->values[v]);
                    break;
                case 2:
                {
                    const vec2 &val = static_cast<vertex_attrib_vec2 *>(va)->values[v];
                    valstr = QString("(%1, %2)").arg(val.x).arg(val.y);
                    break;
                }
                case 3:
                {
                    const vec3 &val = static_cast<vertex_attrib_vec3 *>(va)->values[v];
                    valstr = QString("(%1, %2, %3)").arg(val.x).arg(val.y).arg(val.z);
                    break;
                }
                case 4:
                {
                    const vec4 &val = static_cast<vertex_attrib_vec4 *>(va)->values[v];
                    valstr = QString("(%1, %2, %3, %4)").arg(val.x).arg(val.y).arg(val.z).arg(val.w);
                    break;
                }
            }

            QTreeWidgetItem *sqtwi = new QTreeWidgetItem(qtwi, QStringList({ va->name, valstr }));
            qtwi->addChild(sqtwi);
        }
    }
}



void stage_tab::apply_shaders(void)
{
    if (!rpd.update_shaders(this))
        return;

    scan_shaders();

    rpd.update_uniforms(this);
    rpd.update_vertex_buffers(this);
    rpd.update_fbo(this);
}

void stage_tab::enable_gsh(int enable)
{
    gsh_edit.setEnabled(enable);
}

void stage_tab::assign_uniform(void)
{
    if (uniform_tree.sel_item == NULL)
        return;

    uniform *u = static_cast<uniform *>(uniform_tree.sel_item->data(0, Qt::UserRole).value<void *>());

    bool ok = false;

    if (u->type == uniform::t_int)
    {
        int val = QInputDialog::getInt(this, "Change uniform value", "New integer value to be assigned to this uniform:", static_cast<int_uniform *>(u)->get(), INT_MIN, INT_MAX, 1, &ok);

        if (ok)
            static_cast<int_uniform *>(u)->set(val);
    }
    else if (u->type == uniform::t_sampler2d)
    {
        managed_texture *val = texture_dialog::get_tex(this, "Change uniform value", "New texture to be assigned to this sampler:", &ok);

        if (ok && (static_cast<sampler2d_uniform *>(u)->val != val))
        {
            if (static_cast<sampler2d_uniform *>(u)->val != NULL)
                unuse_texture(static_cast<sampler2d_uniform *>(u)->val);

            use_texture(val);

            sweep_textures();

            static_cast<sampler2d_uniform *>(u)->set(val);
        }
    }
    else if (u->type == uniform::t_float)
    {
        float val = QInputDialog::getDouble(this, "Change uniform value", "New floating point value to be assigned to this uniform:", static_cast<float_uniform *>(u)->get(), -HUGE_VAL, HUGE_VAL, 10, &ok);

        if (ok)
            static_cast<float_uniform *>(u)->set(val);
    }
    else if (u->type == uniform::t_vec2)
    {
        vec2 val = vector_dialog::get_vec2(this, "Change uniform value", "New value to be assigned to this uniform vector:", static_cast<vec2_uniform *>(u)->get(), &ok);

        if (ok)
            static_cast<vec2_uniform *>(u)->set(val);
    }
    else if (u->type == uniform::t_vec3)
    {
        vec3 val = vector_dialog::get_vec3(this, "Change uniform value", "New value to be assigned to this uniform vector:", static_cast<vec3_uniform *>(u)->get(), &ok);

        if (ok)
            static_cast<vec3_uniform *>(u)->set(val);
    }
    else if (u->type == uniform::t_vec4)
    {
        vec4 val = vector_dialog::get_vec4(this, "Change uniform value", "New value to be assigned to this uniform vector:", static_cast<vec4_uniform *>(u)->get(), &ok);

        if (ok)
            static_cast<vec4_uniform *>(u)->set(val);
    }
    else if (u->type == uniform::t_mat2)
    {
        mat2 val = matrix_dialog::get_mat2(this, "Change uniform value", "New value to be assigned to this uniform matrix:", static_cast<mat2_uniform *>(u)->get(), &ok);

        if (ok)
            static_cast<mat2_uniform *>(u)->set(val);
    }
    else if (u->type == uniform::t_mat3)
    {
        mat3 val = matrix_dialog::get_mat3(this, "Change uniform value", "New value to be assigned to this uniform matrix:", static_cast<mat3_uniform *>(u)->get(), &ok);

        if (ok)
            static_cast<mat3_uniform *>(u)->set(val);
    }
    else if (u->type == uniform::t_mat4)
    {
        mat4 val = matrix_dialog::get_mat4(this, "Change uniform value", "New value to be assigned to this uniform matrix:", static_cast<mat4_uniform *>(u)->get(), &ok);

        if (ok)
            static_cast<mat4_uniform *>(u)->set(val);
    }

    if (ok)
        uniform_tree.sel_item->setText(1, u->valstr);
}

void stage_tab::bind_builtin(void)
{
    if (uniform_tree.sel_item == NULL)
        return;

    uniform *u = static_cast<uniform *>(uniform_tree.sel_item->data(0, Qt::UserRole).value<void *>());

    bool ok;
    trackable *t = trackable_dialog::get_track(this, "Change uniform binding", "Builtin this uniform should be bound to:", u->type, &ok);

    if (ok)
    {
        u->track = t->ptr;

        // FIXME: But the whole trackable thing is kind of broken; though not too bad.
        if (static_cast<void *>(t->ptr) == static_cast<void *>(render->it_modelview->d))
            u->transposed = true;
        else
            u->transposed = false;

        u->valstr = "Built-in: " + t->name;
        uniform_tree.sel_item->setText(1, u->valstr);
    }
}


void stage_tab::different_vertex_rendering_method(int idx)
{
    vertex_rendering_method = vtx_type.itemData(idx).value<int>();
}
