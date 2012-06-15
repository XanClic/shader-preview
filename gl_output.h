#ifndef GL_OUTPUT_H
#define GL_OUTPUT_H

#include <GL/gl.h>
#include <GL/glu.h>

#include <QtOpenGL>
#include <QGLWidget>

class gl_output: public QGLWidget
{
    Q_OBJECT

    public:
        gl_output(QWidget *parent):
            QGLWidget(parent),
            rotate_object(false)
        {
            connect(QAbstractEventDispatcher::instance(), SIGNAL(aboutToBlock()), this, SLOT(OnIdle()));
        }

    public slots:
        void OnIdle()
        {
            update();
        }

    protected:
        void initializeGL(void)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            glClearColor(0.f, 0.f, 0.f, 0.f);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(0.f, 0.f, -3.f);

            quad_list = glGenLists(1);

            float   vertices[] = { 1.f, 1.f, -1.f, 1.f, -1.f, -1.f, 1.f, -1.f };
            float tex_coords[] = { 1.f, 1.f,  0.f, 1.f,  0.f,  0.f, 1.f,  0.f };
            glVertexPointer  (2, GL_FLOAT, 0, vertices);
            glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);

            glNewList(quad_list, GL_COMPILE);
            glColor3f(1.f, 1.f, 1.f);
            glDrawArrays(GL_QUADS, 0, 4);
            glEndList();
        }

        void resizeGL(int w, int h)
        {
            glViewport(0, 0, w, h);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.f, static_cast<float>(w) / static_cast<float>(h), .1f, 10.f);

            glMatrixMode(GL_MODELVIEW);
        }

        void paintGL(void)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glCallList(quad_list);
        }

        void mousePressEvent(QMouseEvent *evt)
        {
            if (evt->button() == Qt::LeftButton)
            {
                grabMouse(Qt::ClosedHandCursor);

                rotate_object = true;

                rot_l_x = evt->x();
                rot_l_y = evt->y();
            }
        }

        void mouseReleaseEvent(QMouseEvent *evt)
        {
            if (evt->button() == Qt::LeftButton)
            {
                releaseMouse();

                rotate_object = false;
            }
        }

        void mouseMoveEvent(QMouseEvent *evt)
        {
            if (!rotate_object)
                return;

            glRotatef(evt->x() - rot_l_x, 0.f, 1.f, 0.f);
            glRotatef(evt->y() - rot_l_y, 1.f, 0.f, 0.f);

            rot_l_x = evt->x();
            rot_l_y = evt->y();
        }

    private:
        GLuint quad_list;
        bool rotate_object;
        int rot_l_x, rot_l_y;
};

#endif
