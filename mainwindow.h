#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCoreApplication>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <QMainWindow>
#include "GLWidget.h"
#include <QtGui>
#include "ui_mainwindow.h"


typedef struct CvArPattern{
    CvPoint     centroid;
    double      areaRelation;
    double      periRelation;
    bool        cornerAt[16];
    CvSize      tam;
}
CvArPattern;

typedef struct CvArPattFound{
    CvPoint esquinas[4];
    int     numPatt;
}
CvArPattFound;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    CvArPattern patrones[12];
    void preProcess(IplImage *camera, IplImage *gray, int tipo);
    void posProcess(IplImage *gray, IplImage *camera, int *cantDetectedPatts, CvArPattFound *detPatts);
    void renProcess(CvArPattFound *detPatts, int cantDetectedPatts, IplImage * camara);
    double zoom;
private:
    CvCapture   *camCapture;
    IplImage    *camara;
    IplImage    *grises;
    GLWidget    *imActualGLWidget;

    int timerId;
    void readCam();
    void stopCam();

public slots:
    void timerEvent(QTimerEvent *);

private slots:
    void on_actionDecrease_Zoom_triggered();
    void on_actionIncrease_Zoom_triggered();
    void on_actionExit_triggered();
    void on_actionStart_triggered();
};

#endif // MAINWINDOW_H
