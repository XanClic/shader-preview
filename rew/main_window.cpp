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
    add_stage_btn("Add stage")
{
    setWindowTitle("Shader Preview");


    tabs.addTab(&render_page, "Result");

    tabs.setCornerWidget(&add_stage_btn);

    connect(&add_stage_btn, SIGNAL(clicked()), this, SLOT(add_stage()));


    render_layout.addWidget(&render);
    render_page.setLayout(&render_layout);


    main_layout.addWidget(&tabs);
    setLayout(&main_layout);


    while (!gl_initialized)
        QCoreApplication::processEvents();

    stage_tab *st1 = new stage_tab(0, &render);
    stage_tabs.push_back(st1);

    tabs.addTab(st1, "Stage 0");


    render.tex_bound = (*st1->outputs)[0]->mt;
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

    tabs.addTab(nst, "Stage " + QString::number(stage_numbers++));
}
