#include <QtGui/QMouseEvent>
#include "GLWidget.h"

//GLfloat light_diffuse[] = {0.2, 0.2, 0.2, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {0.0, 0.0, 1.0, 1.0};  /* Infinite light location. */

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
    this->img = 0;
    this->cantDetectedPatts = 0;
}

void GLWidget::initializeGL() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h); // set origin to bottom left corner
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void GLWidget::paintGL() {

    if(this->img)
    {

        glClearColor (1.0,1.0,1.0,1.0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glClearDepth(1.0f);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0,img->width,img->height,0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
//        glEnable(GL_TEXTURE_2D);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexImage2D( GL_TEXTURE_2D, 0, 4, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->imageData );
//        glBegin(GL_QUADS);
//        glTexCoord2f(0,0); glVertex2f(0,img->height);
//        glTexCoord2f(0,1); glVertex2f(0,0);
//        glTexCoord2f(1,1); glVertex2f(img->width,0);
//        glTexCoord2f(1,0); glVertex2f(img->width,img->height);
//        glEnd();
//        glDisable(GL_TEXTURE_2D);
//        glFlush();
        glDrawPixels(img->width,img->height,GL_RGB,GL_UNSIGNED_BYTE,img->imageData);

        if (this->cantDetectedPatts > 0)
        {
            glEnable(GL_LIGHTING);
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);
            glEnable(GL_LIGHT0);

            glShadeModel(GL_SMOOTH);

            glEnable(GL_COLOR_MATERIAL);
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
            glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE) ;

            glClearDepth(1.0);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
//            glAlphaFunc(GL_GREATER,0.1);
//            glEnable(GL_ALPHA_TEST);
//            glEnable(GL_TEXTURE_2D);
//            glEnable(GL_CULL_FACE);

            for(int i=0;i<this->cantDetectedPatts;i++)

            {
                GLdouble mAct[16];
                for(int j=0;j<16;j++)
                {
                    mAct[j] = this->model[i][j];
                }
                int actPat = this->detPatts[i];

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glLoadMatrixd(this->proje);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glLoadMatrixd(mAct);

                glBegin(GL_TRIANGLES);

                for(unsigned j=0;j<this->mallas[actPat]->numIndices()*3;j+=3)
                {
                    glColor4f(this->mallas[actPat]->materials[3*(this->mallas[actPat]->faces[j/3].material)],this->mallas[actPat]->materials[3*(this->mallas[actPat]->faces[j/3].material)+1],this->mallas[actPat]->materials[3*(this->mallas[actPat]->faces[j/3].material)+2],1.0f - this->mallas[actPat]->matalpha[this->mallas[actPat]->faces[j/3].material]);
                    glNormal3f(this->mallas[actPat]->facenormals[j], this->mallas[actPat]->facenormals[j+1], this->mallas[actPat]->facenormals[j+2]);
                    glVertex3fv(this->mallas[actPat]->faces[j/3].vertex[0].points);
                    glVertex3fv(this->mallas[actPat]->faces[j/3].vertex[1].points);
                    glVertex3fv(this->mallas[actPat]->faces[j/3].vertex[2].points);
                }
                glEnd();

                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
            }
        }
     }
}

void GLWidget::setModelProject(GLdouble p[16],GLdouble m[20][16],int indDetected[20], int cantDetected)
{
    this->cantDetectedPatts = cantDetected;
    for(int i=0;i<20;i++)
    {
        this->detPatts[i] = indDetected[i];
        for(int j=0;j<16;j++)
        {
            this->model[i][j] = m[i][j];
        }
    }
    for(int j=0;j<16;j++)
    {
        this->proje[j] = p[j];
    }
    this->updateGL();
}

void GLWidget::setImage(IplImage *imagen)
{
    this->img = imagen;
}

void GLWidget::mousePressEvent(QMouseEvent *event) {

}


void GLWidget::mouseMoveEvent(QMouseEvent *event) {
//    printf("%d, %d\n", event->x(), event->y());
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_Escape:
        close();
        break;
    default:
        event->ignore();
        break;
    }
}


Mesh *GLWidget::ParseMesh(QString filename,bool *success)
{
    *success = true;
    QFile       file(filename);
    QFileInfo   fileInfo(file);
    if(!file.exists())
    {
        *success=false;
        return &Mesh();
    }
    if(!file.open(QIODevice::ReadOnly))
    {
        *success=false;
        return &Mesh();
    }

    unsigned vertices=0;
    unsigned texturevertex=0;
    unsigned normals=0;
    unsigned faces=0;
    QString materialFileName="";

    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QList<QByteArray> values = line.simplified().split(' ');
        if(values.at(0)==QString("v"))
            vertices++;
        if(values.at(0)==QString("vn"))
            ++normals;
        if(values.at(0)==QString("vt"))
            ++texturevertex;
        if(values.at(0)==QString("f"))
        {
            faces++;
            if(values.length()==5)
            {
            faces++;
            }
        }
//        if(values.at(0)==QString("g"))
//            qDebug()<<"Sorry we do not support groups for now, you can contribute doing it ;)";
        if(line.indexOf("mtllib")!=-1)
        {
            if(values.length()==2)
            {
                materialFileName = values.at(1);
            }
            else
            {
//                qDebug()<<"The file name for material library can't have spaces";
            }
        }
    }

    materialFileName = fileInfo.absolutePath() + "/" + materialFileName;
    QFile materialFile(materialFileName);

    int numMaterials = 0;
    int actMaterial  = 0;
    meshMaterial *materiales;

    if(!materialFile.exists())
    {
//        QMessageBox::warning(0,"Error","No se puede encontrar el archivo de materiales: " + materialFileName);
//        success=false;
//        return &Mesh();

    }
    else
    {
        materialFile.open(QIODevice::ReadOnly);
        while(!materialFile.atEnd())
        {
            QByteArray line = materialFile.readLine();
            QList<QByteArray> values = line.simplified().split(' ');
            if(values.at(0) == QString("newmtl"))
            {
            numMaterials++;
            }
        }
        numMaterials++;
        materialFile.reset();
        materiales = new meshMaterial[numMaterials];

        materiales[0].color[0] = 0.7;
        materiales[0].color[1] = 0.7;
        materiales[0].color[2] = 0.7;

        while(!materialFile.atEnd())
        {
            QByteArray line = materialFile.readLine();
            QList<QByteArray> values = line.simplified().split(' ');

            if(values.at(0) == QString("newmtl"))
            {
                actMaterial++;
                materiales[actMaterial].matName = values.at(1);
            }

            if(values.at(0).toUpper() == QString("TR"))
            {
                materiales[actMaterial].alpha = values.at(1).toFloat();
            }
            if(values.at(0).toUpper() == QString("KD"))
            {
                materiales[actMaterial].color[0] = values.at(1).toFloat();
                materiales[actMaterial].color[1] = values.at(2).toFloat();
                materiales[actMaterial].color[2] = values.at(3).toFloat();
            }



        }
        materialFile.close();
    }

    if(!vertices)
    *success=false;

    file.reset();
    Mesh *mesh = new Mesh(vertices,faces,texturevertex,normals,numMaterials);
//    qDebug() << "La malla tiene " << vertices << " vertices, " << faces << " caras, " << normals << " normales" ;
    float        *v  = mesh->vertices;
    float        *vt = mesh->textcords;
    float        *vn = mesh->vertexnormals;
    unsigned int *f  = mesh->indices;
    unsigned int *fm = mesh->facematerials;
    float        *m  = mesh->materials;
    float        *ma = mesh->matalpha;

//    qDebug() << "Cantidad de Materiales:" << numMaterials;

    for (int i=0;i<numMaterials*3;i+=3)
    {
        *m = materiales[i/3].color[0]; m++;
        *m = materiales[i/3].color[1]; m++;
        *m = materiales[i/3].color[2]; m++;
        *ma= materiales[i/3].alpha; ma++;
    }

    int actualMaterial = 0;

    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QList<QByteArray> values = line.simplified().split(' ');
        if(values.at(0)==QString("v"))
        {
            for(int i =1;i<=3;i++)
            {
                *v=(float)atof(values.at(i));
                v++;
            }
        }
        if(values.at(0)==QString("vn"))
        {
            for(int i=1;i<=3;i++)
            {
                *vn=(float)atof(values.at(i));
                vn++;
            }
        }
        if(values.at(0)==QString("vt"))
        {
            for(int i=1;i<=3;i++)
            {
                *vt=(float)atof(values.at(i));
                vt++;
            }
        }
        if(values.at(0)==QString("usemtl"))
        {
            QString actMat = values.at(1);
            if (actMat != QString("(null)"))
            {
                for (int i=0;i<numMaterials;i++)
                {
                    if(actMat == materiales[i].matName)
                    {
                        actualMaterial = i;
                        break;
                    }
                }
            }
            actMat.detach();
        }
        if(values.at(0)==QString("f"))
        {

            for(int i=1;i<=3;i++)
            {
                *f=atoi(values.at(i))-1;
                f++;
            }
            *fm = actualMaterial;
            fm++;
            if(values.length()==5)
            {
                *f=atoi(values.at(3))-1;
                f++;
                *f=atoi(values.at(4))-1;
                f++;
                *f=atoi(values.at(1))-1;
                f++;

                *fm = actualMaterial;
                fm++;
            }
        }
    }
    mesh->generateFaceNormals();

//    if (normals != vertices)
//    mesh->generateVertexNormals();

    mesh->separateFaces();
    file.close();
//    delete[] materiales;
    return mesh;
    fileInfo.~QFileInfo();
    file.~QFile();
    materialFile.~QFile();

}
