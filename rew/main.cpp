#include <cstdio>

#include <QApplication>
#include <QTextCodec>

#include "main_window.hpp"


int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QApplication app(argc, argv);

    int res;

    try
    {
        main_window wnd_main;
        wnd_main.resize(800, 600);
        wnd_main.show();

        res = app.exec();
    }
    catch (int exc)
    {
        res = 1;
        fprintf(stderr, "Caught barely-implemented exception #%i.\n", exc);
    }

    return res;
}
