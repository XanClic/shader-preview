#include <QtCore>
#include <QtGui>

#include "shader_edit.hpp"


shader_edit::shader_edit(QWidget *parent):
    QPlainTextEdit(parent)
{
}

void shader_edit::contextMenuEvent(QContextMenuEvent *evt)
{
    QMenu *menu = createStandardContextMenu();

    menu->addSeparator();
    connect(menu->addAction("Load file..."), SIGNAL(triggered()), this, SLOT(load_file()));
    connect(menu->addAction("Save"), SIGNAL(triggered()), this, SLOT(just_save()));
    connect(menu->addAction("Save file..."), SIGNAL(triggered()), this, SLOT(save_file()));

    menu->exec(evt->globalPos());

    delete menu;
}


void shader_edit::load_file(void)
{
    QString out = QFileDialog::getOpenFileName(this, "Open shader file");
    if (out.isEmpty())
        return;

    QFile fp(out);
    if (!fp.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error loading file", "Could not open the given file: " + fp.errorString());
        return;
    }

    file_name = out;

    QByteArray data = fp.readAll();

    if (fp.error() != QFile::NoError)
    {
        QMessageBox::critical(this, "Error loading file", "An error occured while loading the file: " + fp.errorString());
        return;
    }

    setPlainText(QString::fromUtf8(data.constData()));
}

void shader_edit::save_file(void)
{
    QString out = QFileDialog::getSaveFileName(this, "Save shader file");
    if (out.isEmpty())
        return;

    QFile fp(out);
    if (!fp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error writing file", "Could not open the file: " + fp.errorString());
        return;
    }

    file_name = out;

    fp.write(toPlainText().toUtf8());

    if (fp.error() != QFile::NoError)
    {
        QMessageBox::critical(this, "Error writing file", "An error occured while writing to the given file: " + fp.errorString());
        return;
    }
}

void shader_edit::just_save(void)
{
    if (file_name.isEmpty())
    {
        save_file();
        return;
    }

    QFile fp(file_name);
    if (!fp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error writing file", "Could not open the file: " + fp.errorString());
        save_file();
        return;
    }

    fp.write(toPlainText().toUtf8());

    if (fp.error() != QFile::NoError)
    {
        QMessageBox::critical(this, "Error writing file", "An error occured while writing to the given file: " + fp.errorString());
        return;
    }
}
