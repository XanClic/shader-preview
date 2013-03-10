#include <cstdio>

#include <QApplication>
#include <QTextCodec>

#include "main_window.hpp"


int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QApplication app(argc, argv);


    bool force_21 = false;

    for (int i = 1; i < argc; i++)
        if (!strcmp(argv[i], "--2.1"))
            force_21 = true;


    int res;

    try
    {
        main_window wnd_main(force_21 ? 2 : -1, force_21 ? 1 : -1);
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
