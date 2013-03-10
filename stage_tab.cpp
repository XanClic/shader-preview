#include "opengl.hpp"

#include <vector>

#include <glibmm.h>
#include <gtkmm.h>

#include "mtk.hpp"

#include "hlcstr.hpp"
#include "shader_edit.hpp"
#include "stage_tab.hpp"


using namespace Gtk;
using namespace Glib;
using namespace std;


extern int ogl_maj, ogl_min;


stage_tab::stage_tab(int sn, renderer *rnd, Notebook &parent):
    gsh_enable("Enable _geometry shader", true),
    render(rnd),
    stage_number(sn)
{
    gsh_enable.signal_clicked().connect(sigc::mem_fun(*this, &stage_tab::enable_gsh));
    gsh_enable.set_size_request(100, 20);


    VBox *gsh_layout = manage(new VBox(false, 5));

    gsh_layout->add(gsh_edit);
    gsh_layout->pack_end(gsh_enable, false, false);

    gsh_layout->set_size_request(100, 50);


    Mtk::HPaned editables;

    editables.add(vsh_edit);
    editables.add(*gsh_layout);
    editables.add(fsh_edit);


    Button *sh_apply = manage(new Button("Appl_y shaders", true));

    sh_apply->signal_clicked().connect(sigc::mem_fun(*this, &stage_tab::apply_shaders));


    VBox *edit_apply = manage(new VBox(false, 5));

    edit_apply->add(editables);
    edit_apply->pack_end(*sh_apply, false, false);

    add(*edit_apply);


    uniform_tree_model = ListStore::create(uniform_tree_cols);

    uniform_tree.set_model(uniform_tree_model);
    uniform_tree.append_column("Uniform", uniform_tree_cols.name);
    uniform_tree.append_column("Value",   uniform_tree_cols.value);

    uniform_tree.set_headers_visible(true);
    uniform_tree.set_size_request(100, 30);

    add(uniform_tree);


    VBox *input_data = manage(new VBox(false, 5));

    vertex_tree_model = TreeStore::create(vertex_tree_cols);

    vertex_tree.set_model(vertex_tree_model);
    vertex_tree.append_column("Attribute", vertex_tree_cols.name);
    vertex_tree.append_column("Value",     vertex_tree_cols.name);

    vertex_tree.set_headers_visible(true);
    vertex_tree.set_size_request(100, 30);

    input_data->add(vertex_tree);


    vtx_mode_model = ListStore::create(vtx_mode_cols);

    struct vmr { int gl; const char *name; };
    for (vmr foo: {
            (vmr){ GL_POINTS, "Points" },
            (vmr){ GL_LINES, "Lines" },
            (vmr){ GL_LINE_STRIP, "Line strip" },
            (vmr){ GL_LINE_LOOP, "Line loop" },
            (vmr){ GL_TRIANGLES, "Triangles" },
            (vmr){ GL_TRIANGLE_STRIP, "Triangle strip" },
            (vmr){ GL_TRIANGLE_FAN, "Triangle fan" },
            (vmr){ GL_QUADS, "Quads" },
            (vmr){ GL_QUAD_STRIP, "Quad strip" }
        })
    {
        TreeModel::Row r = *vtx_mode_model->append();
        r[vtx_mode_cols.gl] = foo.gl;
        r[vtx_mode_cols.name] = foo.name;
    }

    vtx_mode.set_model(vtx_mode_model);
    vtx_mode.set_entry_text_column(vtx_mode_cols.name);
    vtx_mode.pack_start(vtx_mode_cols.name);

    vtx_mode.set_active(7);
    vertex_rendering_method = GL_QUADS;

    vtx_mode.signal_changed().connect(sigc::mem_fun(*this, &stage_tab::different_vertex_rendering_method));

    input_data->add(vtx_mode);


    Mtk::HPaned bottom_layout;

    bottom_layout.add(*input_data);


    buffer_tree_model = ListStore::create(buffer_tree_cols);

    buffer_tree.set_model(buffer_tree_model);
    buffer_tree.append_column("Color buffer", buffer_tree_cols.name);

    buffer_tree.set_headers_visible(true);
    buffer_tree.set_size_request(50, 30);

    bottom_layout.add(buffer_tree);


    add(bottom_layout);


    HBox *tab_label = manage(new HBox(false, 5));
    tab_label->pack_start(*manage(new Label("Stage %i"_hl % stage_number)), true, true);
    tab_label->pack_end(*manage(new Image(StockID("gtk-close"), ICON_SIZE_MENU)), false, false);

    tab_label->show_all();


    parent.append_page(*this, *tab_label);



    uniforms = new vector<uniform *>;
    vertices = new vertex_info;

    outputs = new vector<color_buffer *>;
    depth = new depth_buffer("Stage %i"_hl % stage_number);


    gsh_edit.set_sensitive(false);
    if (ogl_maj < 3)
        gsh_enable.set_sensitive(false);


    if (ogl_maj >= 3)
    {
        if ((ogl_maj >= 4) || (ogl_min >= 2))
        {
            vsh_edit.buffer()->assign("#version 150 core\n\nin vec4 vertex;\n\nuniform mat4 projection, modelview;\n\nvoid main(void)\n{\n    gl_Position = projection * modelview * vertex;\n}");
            fsh_edit.buffer()->assign("#version 150 core\n\nout vec4 color;\n\nvoid main(void)\n{\n    color = vec4(1.0, 0.25, 0.0, 1.0);\n}");
        }
        else
        {
            vsh_edit.buffer()->assign("#version 130\n\nin vec4 vertex;\n\nuniform mat4 projection, modelview;\n\nvoid main(void)\n{\n    gl_Position = projection * modelview * vertex;\n}");
            fsh_edit.buffer()->assign("#version 130\n\nout vec4 color;\n\nvoid main(void)\n{\n    color = vec4(1.0, 0.25, 0.0, 1.0);\n}");
        }
    }
    else
    {
        vsh_edit.buffer()->assign("#version 110\n\nuniform mat4 projection, modelview;\n\nvoid main(void)\n{\n    gl_Position = projection * modelview * gl_Vertex;\n}");
        fsh_edit.buffer()->assign("#version 110\n\nvoid main(void)\n{\n    gl_FragColor = vec4(1.0, 0.25, 0.0, 1.0);\n}");
    }


    if (!rpd.update_shaders(this)) throw 42; // FIXME


    scan_shaders();

    vertex_attrib_vec4 *va = static_cast<vertex_attrib_vec4 *>(vertices->attribs[0]);
    va->values.push_back(vec4(-1.f,  1.f, 0.f, 1.f));
    va->values.push_back(vec4(-1.f, -1.f, 0.f, 1.f));
    va->values.push_back(vec4( 1.f, -1.f, 0.f, 1.f));
    va->values.push_back(vec4( 1.f,  1.f, 0.f, 1.f));

    for (int i = 1; i < (int)vertices->attribs.size(); i++)
    {
        vertex_attrib *nva = vertices->attribs[i];

        for (int j = 0; j < 4; j++)
        {
            switch (nva->epv)
            {
                case 1: static_cast<vertex_attrib_float *>(nva)->values.push_back(0.f); break;
                case 2: static_cast<vertex_attrib_vec2  *>(nva)->values.push_back(vec2(0.f, 0.f)); break;
                case 3: static_cast<vertex_attrib_vec3  *>(nva)->values.push_back(vec3(0.f, 0.f, 0.f)); break;
                case 4: static_cast<vertex_attrib_vec4  *>(nva)->values.push_back(vec4(0.f, 0.f, 0.f, 0.f)); break;
            }
        }
    }

    (*uniforms)[0]->track = render->projection;
    (*uniforms)[1]->track  = render->modelview;

    (*uniforms)[0]->valstr = "Built-in: Projection matrix";
    (*uniforms)[1]->valstr  = "Built-in: Modelview matrix";

    scan_shaders();

    rpd.update_fbo(this);
    rpd.update_uniforms(this);
    rpd.update_vertex_buffers(this);
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

    delete vertices;


    for (color_buffer *cb: *outputs)
        delete cb;

    delete outputs;

    delete depth;
}


static uniform::utype get_type_from_string(const ustring &str)
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

static int get_type_elements_from_string(const ustring &str)
{
    if (str == "float")
        return 1;
    else if (str == "vec2")
        return 2;
    else if (str == "vec3")
        return 3;
    else if (str == "vec4")
        return 4;
    else
        return -1;
}

void stage_tab::scan_shaders(void)
{
    uniform_tree_model.clear();
    vertex_tree_model.clear();
    buffer_tree_model.clear();


    ustring vsh_src = vsh_edit.buffer()->get_text();
    ustring gsh_src = gsh_edit.get_sensitive() ? gsh_edit.buffer()->get_text() : "";
    ustring fsh_src = fsh_edit.buffer()->get_text();

    if (ogl_maj < 3)
    {
        // TODO: Multitexturing
        vsh_src += "\nin vec4 gl_Vertex, gl_MultiTexCoord0, gl_Color;\nin vec3 gl_Normal;";
        // TODO: gl_FragData[i]
        fsh_src += "\nout vec4 gl_FragColor;";
    }

    vector<ustring> vsh_split = Regex::split_simple("[;{}]", vsh_src, REGEX_MULTILINE, REGEX_MATCH_NOTEMPTY);
    vector<ustring> gsh_split = Regex::split_simple("[;{}]", gsh_src, REGEX_MULTILINE, REGEX_MATCH_NOTEMPTY);
    vector<ustring> fsh_split = Regex::split_simple("[;{}]", fsh_src, REGEX_MULTILINE, REGEX_MATCH_NOTEMPTY);

    vector<ustring> uniform_defs, vbo_defs, output_defs;

    vector<ustring> *splits[3] = { &vsh_split, &gsh_split, &fsh_split };

    for (vector<ustring> *split: splits)
    {
        for (ustring s: *split)
        {
            ustring done = s;

            do
            {
                done = Regex::create("^#[^\\n]*"    )->replace(done, 0, "", RegexMatchFlags(0));
                done = Regex::create("^[[:space:]]*")->replace(done, 0, "", RegexMatchFlags(0));
                done = Regex::create("[[:space:]]*$")->replace(done, 0, "", RegexMatchFlags(0));

            }
            while (done[0] == '#');

            if (!done[0])
                continue;

            if (Regex::match_simple("^\\s*(layout\\s+\\(.*\\)\\s|)\\s*uniform\\s+", done))
                uniform_defs.push_back(Regex::create("^\\s*(layout\\s+\\(.*\\)\\s|)\\s*uniform\\s+")->replace(done, 0, "", RegexMatchFlags(0)));
            else if ((split == &vsh_split) && Regex::match_simple("^\\s*(layout\\s+\\(.*\\)\\s|)\\s*in\\s+", done))
                vbo_defs.push_back(Regex::create("^\\s*(layout\\s+\\(.*\\)\\s|)\\s*in\\s+")->replace(done, 0, "", RegexMatchFlags(0)));
            else if ((split == &fsh_split) && Regex::match_simple("^\\s*(layout\\s+\\(.*\\)\\s|)\\s*out\\s+", done))
                output_defs.push_back(Regex::create("^\\s*(layout\\s+\\(.*\\)\\s|)\\s*out\\s+")->replace(done, 0, "", RegexMatchFlags(0)));
        }
    }


    vector<vertex_attrib *> old_attribs(vertices->attribs);
    vector<uniform *> *new_uniforms = new vector<uniform *>;
    vector<color_buffer *> *new_outputs = new vector<color_buffer *>;

    vector<ustring> *vars[3] = { &uniform_defs, &vbo_defs, &output_defs };

    for (vector<ustring> *var: vars)
    {
        for (ustring s: *var)
        {
            RefPtr<Regex> regex = Regex::create("^([^\\s]+)\\s+(.+)$");
            MatchInfo match;

            if (!regex->match(s, match))
                continue;

            ustring type = match.fetch(1);

            for (ustring identifier: Regex::split_simple("\\s*,\\s*", match.fetch(2), RegexCompileFlags(0), REGEX_MATCH_NOTEMPTY))
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
                            uniforms->erase(std::remove(uniforms->begin(), uniforms->end(), u), uniforms->end());
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
                        {
                            fprintf(stderr, "Unknown uniform type %s.\n", type.data());
                            abort(); // FIXME
                        }

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
                            outputs->erase(std::remove(outputs->begin(), outputs->end(), cb), outputs->end());
                            break;
                        }
                    }

                    if (ncb == NULL)
                    {
                        ncb = new color_buffer("Stage %i"_hl % stage_number, identifier);

                        if (used_in_display)
                            ncb->resize(render->width, render->height);
                    }

                    new_outputs->push_back(ncb);
                }
                else if (var == &vbo_defs)
                {
                    vertex_attrib *nva = NULL;

                    for (vertex_attrib *va: vertices->attribs)
                    {
                        if ((va->name == identifier) && (va->epv == get_type_elements_from_string(type)))
                        {
                            nva = va;
                            old_attribs.erase(std::remove(old_attribs.begin(), old_attribs.end(), va), old_attribs.end());
                            break;
                        }
                    }

                    if (nva == NULL)
                    {
                        if (type == "float")
                            nva = new vertex_attrib_float(identifier);
                        else if (type == "vec2")
                            nva = new vertex_attrib_vec2(identifier);
                        else if (type == "vec3")
                            nva = new vertex_attrib_vec3(identifier);
                        else if (type == "vec4")
                            nva = new vertex_attrib_vec4(identifier);
                        else
                        {
                            fprintf(stderr, "Unknown VBO type %s.\n", type.data());
                            abort(); // FIXME
                        }

                        if (vertices->attribs.size())
                        {
                            int len = vertices->attribs[0]->len();
                            for (int i = 0; i < len; i++)
                            {
                                switch (nva->epv)
                                {
                                    case 1: static_cast<vertex_attrib_float *>(nva)->values.push_back(0.f); break;
                                    case 2: static_cast<vertex_attrib_vec2  *>(nva)->values.push_back(vec2(0.f, 0.f)); break;
                                    case 3: static_cast<vertex_attrib_vec3  *>(nva)->values.push_back(vec3(0.f, 0.f, 0.f)); break;
                                    case 4: static_cast<vertex_attrib_vec4  *>(nva)->values.push_back(vec4(0.f, 0.f, 0.f, 0.f)); break;
                                }
                            }
                        }

                        vertices->attribs.push_back(nva);
                    }
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


    for (vertex_attrib *va: old_attribs)
    {
        vertices->attribs.erase(std::remove(vertices->attribs.begin(), vertices->attribs.end(), va), vertices->attribs.end());
        delete va;
    }

    vertices->update_buffer();


    TreeModel::Row r;

    for (uniform *u: *uniforms)
    {
        r = *uniform_tree_model->append();
        r[uniform_tree_cols.uni] = u;
        r[uniform_tree_cols.name] = u->name;
        r[uniform_tree_cols.value] = u->valstr;
    }

    for (color_buffer *cb: *outputs)
    {
        r = *buffer_tree_model->append();
        r[buffer_tree_cols.cb] = cb;
        r[buffer_tree_cols.name] = cb->name;
    }

    r = *buffer_tree_model->append();
    r[buffer_tree_cols.name] = "depth";

    if (vertices->attribs.size() < 1)
        return;

    int vertex_count = vertices->attribs[0]->len();
    for (int v = 0; v < vertex_count; v++)
    {
        r = *vertex_tree_model->append();
        r[vertex_tree_cols.vi] = v;
        r[vertex_tree_cols.va] = NULL;
        r[vertex_tree_cols.name] = ustring::format(v);
        r[vertex_tree_cols.value] = "";

        for (vertex_attrib *va: vertices->attribs)
        {
            ustring valstr;

            switch (va->epv)
            {
                case 1:
                    valstr = ustring::format(static_cast<vertex_attrib_float *>(va)->values[v]);
                    break;
                case 2:
                {
                    const vec2 &val = static_cast<vertex_attrib_vec2 *>(va)->values[v];
                    valstr = ustring::compose("(%1, %2)", val.x, val.y);
                    break;
                }
                case 3:
                {
                    const vec3 &val = static_cast<vertex_attrib_vec3 *>(va)->values[v];
                    valstr = ustring::compose("(%1, %2, %3)", val.x, val.y, val.z);
                    break;
                }
                case 4:
                {
                    const vec4 &val = static_cast<vertex_attrib_vec4 *>(va)->values[v];
                    valstr = ustring::compose("(%1, %2, %3, %4)", val.x, val.y, val.z, val.w);
                    break;
                }
            }

            TreeModel::Row c = *vertex_tree_model->append(r.children());
            c[vertex_tree_cols.vi] = v;
            c[vertex_tree_cols.va] = va;
            c[vertex_tree_cols.name] = va->name;
            c[vertex_tree_cols.value] = valstr;
        }
    }
}



void stage_tab::apply_shaders(void)
{
    ready_to_roll = false;

    if (!rpd.update_shaders(this))
    {
        ready_to_roll = true;
        return;
    }

    scan_shaders();

    rpd.update_uniforms(this);
    rpd.update_vertex_buffers(this);
    rpd.update_fbo(this);

    ready_to_roll = true;
}

void stage_tab::enable_gsh(void)
{
    gsh_edit.set_sensitive(gsh_enable.get_active());
}

void stage_tab::assign_uniform(void)
{
    RefPtr<TreeSelection> sel = uniform_tree.get_selection();

    uniform *u = (*sel->get_selected())[uniform_tree_cols.uni];

    bool ok = false;

    if (ok)
        (*sel->get_selected())[uniform_tree_cols.value] = u->valstr;
}

void stage_tab::bind_builtin(void)
{
}


void stage_tab::different_vertex_rendering_method(void)
{
    vertex_rendering_method = (*vtx_mode.get_active())[vtx_mode_cols.gl];
}


void stage_tab::set_displayed(bool isit)
{
    if (used_in_display == isit)
        return;

    used_in_display = isit;

    for (color_buffer *cb: *outputs)
        cb->resize(isit ? render->width : -1, isit ? render->height : -1);

    depth->resize(isit ? render->width : -1, isit ? render->height : -1);
}
