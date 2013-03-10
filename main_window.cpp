#include <QtCore>
#include <QtGui>

#include "renderer.hpp"
#include "main_window.hpp"
#include "stage_tab.hpp"


extern volatile bool gl_initialized;

static int stage_numbers = 1;

main_window::main_window(void):
    QWidget(NULL),
    render(this),
    scale_display_fbo("Render displayed FBO in appropriate resolution"),
    add_stage_btn("Add stage")
{
    setWindowTitle("Shader Preview");


    render_page = new QWidget;
    tabs = new QTabWidget;


    tabs->setTabsClosable(true);

    tabs->addTab(render_page, "Result");

    tabs->setCornerWidget(&add_stage_btn);

    connect(&add_stage_btn, SIGNAL(clicked()), this, SLOT(add_stage()));
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(remove_stage(int)));


    scale_display_fbo.setCheckState(Qt::Checked);

    connect(&scale_display_fbo, SIGNAL(stateChanged(int)), &render, SLOT(fbo_display_setting_changed(int)));


    render_layout = new QVBoxLayout;
    main_layout = new QVBoxLayout;


    render_layout->addWidget(&render);
    render_layout->addWidget(&scale_display_fbo);
    render_page->setLayout(render_layout);


    main_layout->addWidget(tabs);
    setLayout(main_layout);


    while (!gl_initialized)
        QCoreApplication::processEvents();

    stage_tab *st1 = new stage_tab(0, &render);
    stage_tabs.push_back(st1);

    tabs->addTab(st1, "Stage 0");


    render.tex_bound = (*st1->outputs)[0]->mt;

    render.fbo_display_setting_changed(Qt::Checked);
}

main_window::~main_window(void)
{
    for (stage_tab *st: stage_tabs)
        delete st;
}


void main_window::add_stage(void)
{
    stage_tab *nst = new stage_tab(stage_numbers, &render);
    stage_tabs.push_back(nst);

    tabs->addTab(nst, "Stage " + QString::number(stage_numbers++));
}

void main_window::remove_stage(int index)
{
    if (!index)
        return;

    stage_tab *st = stage_tabs[index - 1];

    for (color_buffer *cb: *st->outputs)
    {
        if (cb->mt == render.tex_bound)
        {
            QMessageBox::critical(this, "Stage bound", "One output texture of this stage is currently being displayed.\n\nThus it cannot be removed.");
            return;
        }
    }

    stage_tabs.removeAt(index - 1);
    delete st;

    tabs->removeTab(index);
}
