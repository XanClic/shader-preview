#ifndef SHADER_EDIT_HPP
#define SHADER_EDIT_HPP

#include <QtGui>
#include <QString>


class shader_edit: public QPlainTextEdit
{
    Q_OBJECT

    public:
        shader_edit(QWidget *parent = NULL);

    public slots:
        void load_file(void);
        void save_file(void);
        void just_save(void);

    protected:
        void contextMenuEvent(QContextMenuEvent *evt);

    private:
        QString file_name;
};

#endif
