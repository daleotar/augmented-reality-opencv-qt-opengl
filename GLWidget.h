#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <cv.h>
#include <cxcore.h>
#include "Cv3DMesh.h"
#include "mesh.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

typedef struct meshMaterial
{
    QString matName;
    float color[3];
    float alpha;
}
meshMaterial;

class GLWidget : public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:
    GLWidget(QWidget *parent = NULL);
    IplImage *img;

    int cantDetectedPatts;
    GLdouble proje[16];
    GLdouble model[20][16];
    int detPatts[20];
//    Cv3DMesh objetos[12];
    Mesh *mallas[12];

    void setModelProject(GLdouble p[16],GLdouble m[20][16],int indDetected[20], int cantDetected);
    void setImage(IplImage *imagen);
    Mesh *ParseMesh(QString filename,bool *success);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif  /* _GLWIDGET_H */
