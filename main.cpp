#include <QApplication>
#include <QTextCodec>

#include "application.hpp"
#include "gl_output.hpp"


int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QApplication app(argc, argv);

    main_window wnd_main;
    wnd_main.resize(640, 480);
    wnd_main.show();

    return app.exec();
}
