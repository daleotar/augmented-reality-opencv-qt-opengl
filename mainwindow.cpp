#include "mainwindow.h"
#include <QDebug>
#include "functions.h"

int tamElem=3;
int cen=(tamElem-1)/2;
IplConvKernel* estructurante = cvCreateStructuringElementEx(tamElem,tamElem,cen,cen,CV_SHAPE_RECT);
int cantPatrones = 12;

MainWindow::MainWindow(QWidget *parent)
{

    setupUi(this);
    zoom = 1.0;
    camCapture = 0;

    imActualGLWidget = new GLWidget();
    imActualGLWidget->setParent(this);
    imActualGLWidget->setGeometry(0,21,640,480);
    this->setFixedSize(640,501);

    QString actDir = QApplication::applicationDirPath();
    for (int i=0;i<cantPatrones;i++)
    {
        IplImage *img = cvLoadImage((QString("patterns/") + QString("P") + (i < 10 ? QString("0") : QString("")) + QString::number(i) + QString(".png") ).toAscii().data(),CV_LOAD_IMAGE_GRAYSCALE);
        img->origin = CV_ORIGIN_BL;
        cvFlip(img,img,0);
        cvMorphologyEx(img,img,0,estructurante,CV_MOP_DILATE);
        cvPattExtractChar(img,patrones+i);
        bool resultado;
        imActualGLWidget->mallas[i] = imActualGLWidget->ParseMesh(actDir + "/obj/Patt" + (i < 10 ? QString("0") : QString("")) + QString::number(i) + QString(".obj"),&resultado);
    }

}

MainWindow::~MainWindow()
{
    stopCam();
    exit(0);
}

void MainWindow::preProcess(IplImage *camera, IplImage *gray, int tipo)
{
    // Recibe los apuntadores de la imagen de la camara y el apuntador a la imagen en grises donde se guardara la imagen umbralizada
    // Si tipo=0 entonces usa una umbralizacion en 100, si tipo=1 usa el filtro canny;

    cvCvtColor(camera,gray,CV_BGR2GRAY);
    cvSmooth(gray,gray,CV_MEDIAN);

    if (tipo == 1)
    {
        cvCanny(gray,gray,30,150);
        cvMorphologyEx(gray,gray,0,estructurante,CV_MOP_CLOSE);
        cvMorphologyEx(gray,gray,0,estructurante,CV_MOP_CLOSE);
    }
    else
    {
        cvThreshold(gray,gray,100,255,CV_THRESH_BINARY_INV);
    }
}

void MainWindow::posProcess(IplImage *gray, IplImage *camera, int *cantDetectedPatts, CvArPattFound *detPatts)
{
    int maxLevel = 1;
    (*cantDetectedPatts) = 0;

    CvSeq* contours = 0;
    CvSeq* contourAct = 0;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvTreeNodeIterator iterator;

    cvFindContours(gray, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP ,CV_CHAIN_APPROX_NONE);

    if(contours != 0)
    cvInitTreeNodeIterator( &iterator, contours, maxLevel);
    int i = 0;

    double minPerim = 20.0*4.0;
    double maxPerim = 4.0*(double)camera->height;

    while( (contourAct = (CvSeq*)cvNextTreeNode(&iterator))!= 0 )
    {
        double contourActPerim = cvContourPerimeter(contourAct);

        if (contourActPerim > minPerim && contourActPerim < maxPerim && cvCheckRepeat(contourAct) < 3)
        {
            CvMoments centro;
            cvMoments(contourAct,&centro,1);

            double m10=cvGetSpatialMoment(&centro,1,0);
            double m00=cvGetSpatialMoment(&centro,0,0);
            double m01=cvGetSpatialMoment(&centro,0,1);
            CvPoint puntoCentro = cvPoint((int)(m10/m00),(int)(m01/m00));

            const int MAX_CORNERS = 50;
            CvPoint corners[MAX_CORNERS];
            int cornerCount = 0;
            int cantTrueCorners = 0;
            CvPoint trueCorners[MAX_CORNERS];

            cvFindChainCorners(contourAct,corners,&cornerCount);
            CvPoint sortedCorners[cornerCount];
            cvCornerArrangeOrder(corners,cornerCount,puntoCentro,sortedCorners);
            cvFindTrueCorners(sortedCorners,cornerCount,trueCorners,&cantTrueCorners);

            if (cantTrueCorners==4)
            {
                CvRect caja = cvContourBoundingRect(contourAct,1);

                cvSetImageROI(camera,caja);

                CvPoint2D32f q[4];
                CvPoint2D32f p1[4];
                CvPoint2D32f p2[4];
                CvPoint2D32f p3[4];
                CvPoint2D32f p4[4];

                p1[0].x= trueCorners[0].x - caja.x;
                p1[0].y= trueCorners[0].y - caja.y;
                p1[1].x= trueCorners[1].x - caja.x;
                p1[1].y= trueCorners[1].y - caja.y;
                p1[2].x= trueCorners[2].x - caja.x;
                p1[2].y= trueCorners[2].y - caja.y;
                p1[3].x= trueCorners[3].x - caja.x;
                p1[3].y= trueCorners[3].y - caja.y;

                p2[0].x= trueCorners[3].x - caja.x;
                p2[0].y= trueCorners[3].y - caja.y;
                p2[1].x= trueCorners[0].x - caja.x;
                p2[1].y= trueCorners[0].y - caja.y;
                p2[2].x= trueCorners[1].x - caja.x;
                p2[2].y= trueCorners[1].y - caja.y;
                p2[3].x= trueCorners[2].x - caja.x;
                p2[3].y= trueCorners[2].y - caja.y;

                p3[0].x= trueCorners[2].x - caja.x;
                p3[0].y= trueCorners[2].y - caja.y;
                p3[1].x= trueCorners[3].x - caja.x;
                p3[1].y= trueCorners[3].y - caja.y;
                p3[2].x= trueCorners[0].x - caja.x;
                p3[2].y= trueCorners[0].y - caja.y;
                p3[3].x= trueCorners[1].x - caja.x;
                p3[3].y= trueCorners[1].y - caja.y;

                p4[0].x= trueCorners[1].x - caja.x;
                p4[0].y= trueCorners[1].y - caja.y;
                p4[1].x= trueCorners[2].x - caja.x;
                p4[1].y= trueCorners[2].y - caja.y;
                p4[2].x= trueCorners[3].x - caja.x;
                p4[2].y= trueCorners[3].y - caja.y;
                p4[3].x= trueCorners[0].x - caja.x;
                p4[3].y= trueCorners[0].y - caja.y;

                IplImage *fixedSquare = cvCreateImage(cvSize(caja.width+4,caja.height+4),camera->depth,1);
                IplImage *contoSquare = cvCreateImage(cvSize(caja.width+4,caja.height+4),camera->depth,1);

                CvMat* warpMatrix[4];

                for (int j=0;j<4;j++)
                {

                    warpMatrix[j] = cvCreateMat(3,3,CV_32FC1);
                    cvZero(warpMatrix[j]);

                    q[j].x = (j==0 || j==3)? (float)patrones[0].tam.width  : 0.0f;
                    q[j].y = (j==0 || j==1)? (float)patrones[0].tam.height : 0.0f;
                }

                cvGetPerspectiveTransform(p1,q,warpMatrix[0]);
                cvGetPerspectiveTransform(p2,q,warpMatrix[1]);
                cvGetPerspectiveTransform(p3,q,warpMatrix[2]);
                cvGetPerspectiveTransform(p4,q,warpMatrix[3]);

                fixedSquare->origin = IPL_ORIGIN_BL;
                contoSquare->origin = IPL_ORIGIN_BL;

                cvZero(fixedSquare);
                cvZero(contoSquare);

                for(int j=0; j<contourAct->total; j++)
                {
                    CvPoint *puntoAct = (CvPoint *) cvGetSeqElem(contourAct,j);
                    puntoAct->x -= caja.x;
                    puntoAct->x += 2;
                    puntoAct->y -= caja.y;
                    ((uchar *)(contoSquare->imageData + (puntoAct->y)*contoSquare->widthStep))[puntoAct->x]=255;
                }

                CvPoint contoCenter = cvGetImageCenter(contoSquare);
                cvFloodFill(contoSquare,contoCenter,cvScalar(255));
                CvRect cajaFixed = caja;
                cajaFixed.x = 2;
                cajaFixed.y = 2;

                cvSetImageROI(fixedSquare,cajaFixed);

                cvCvtColor(camera,fixedSquare,CV_BGR2GRAY);
                cvSmooth(fixedSquare,fixedSquare,CV_MEDIAN);
                cvResetImageROI(camera);
                cvResetImageROI(fixedSquare);

                cvNot(contoSquare,contoSquare);
                cvOr(fixedSquare,contoSquare,fixedSquare);

                /*int umbral = 100;
                cvGetHistoThresh(fixedSquare,&umbral);
                cvSmooth(fixedSquare,fixedSquare,CV_GAUSSIAN);
                cvThreshold(fixedSquare,fixedSquare,umbral,255,CV_THRESH_BINARY);
*/

                cvCanny(fixedSquare,fixedSquare,30,150);
                cvMorphologyEx(fixedSquare,fixedSquare,0,estructurante,CV_MOP_CLOSE);
                cvMorphologyEx(fixedSquare,fixedSquare,0,estructurante,CV_MOP_CLOSE);

//                cvSetImageROI(fixedSquare,cajaFixed);
//                cvWarpPerspective( fixedSquare, pattSquare[0], warpMatrix[0]);
//                cvWarpPerspective( fixedSquare, pattSquare[1], warpMatrix[1]);
//                cvWarpPerspective( fixedSquare, pattSquare[2], warpMatrix[2]);
//                cvWarpPerspective( fixedSquare, pattSquare[3], warpMatrix[3]);
//                cvResetImageROI(fixedSquare);

                CvArPattern actGenePatt = {0};
                cvInitArPattern(&actGenePatt);
                actGenePatt.tam = patrones[0].tam;
                cvGenericPattExtractChar(fixedSquare,&actGenePatt,contourAct,warpMatrix[0],(double)(patrones[0].tam.width)/5.0);
//                cvShowImage("Fixed",fixedSquare);

                CvArPattern actViewPatt[4] = {{0},{0},{0},{0}};

                for (int j=0;j<4;j++)
                {

                    actViewPatt[j].areaRelation = actGenePatt.areaRelation;
                    actViewPatt[j].periRelation = actGenePatt.periRelation;
                    actViewPatt[j].centroid = actGenePatt.centroid;
                    actViewPatt[j].tam = actGenePatt.tam;
                    for (int k=0; k<16; k++)
                    {
                        actViewPatt[j].cornerAt[k] = actGenePatt.cornerAt[k];
                    }
    //                    actViewPatt[j].img = pattSquare[j];
                    actViewPatt[j].centroid = cvRotatePoint(actViewPatt[j].centroid, cvPoint2D32f( (double)(patrones[0].tam.width)/2,(double)(patrones[0].tam.height)/2) , (double)(j*90));
                    cvArPattRotCorners(actViewPatt+j,j);
    //                    cvCircle(actViewPatt[j].img, actViewPatt[j].centroid, 5, cvScalar(175),2);
                }

                bool matchPatt = false;
                bool matched = false;
                int indVieMatch = 0;
                int indDetPatts = 0;

                for (int j=0;j<cantPatrones;j++)
                {
                    for (int k=0;k<4;k++)
                    {
                        cvMatchArPatt(patrones[j],actViewPatt[k],((double)(patrones[0].tam.width))/5.0,&matchPatt);
                        if(matchPatt)
                        {
                            matched = true;
                            indVieMatch = k;
                            indDetPatts = j;
                            (*cantDetectedPatts)++;
                        }
                    }
                }
                if(matched)
                {
                    (detPatts+((*cantDetectedPatts)-1))->numPatt = indDetPatts;
                    switch(indVieMatch)
                    {
                        case 0:
                        for(int j=0;j<4;j++)
                        {
                            (detPatts+((*cantDetectedPatts)-1))->esquinas[j].x = (int)p1[j].x + caja.x;
                            (detPatts+((*cantDetectedPatts)-1))->esquinas[j].y = (int)p1[j].y + caja.y;
                        }
                        break;
                        case 1:
                        for(int j=0;j<4;j++)
                        {
                            (detPatts+((*cantDetectedPatts)-1))->esquinas[j].x = (int)p2[j].x + caja.x;
                            (detPatts+((*cantDetectedPatts)-1))->esquinas[j].y = (int)p2[j].y + caja.y;
                        }
                        break;
                        case 2:
                        for(int j=0;j<4;j++)
                        {
                            (detPatts+((*cantDetectedPatts)-1))->esquinas[j].x = (int)p3[j].x + caja.x;
                            (detPatts+((*cantDetectedPatts)-1))->esquinas[j].y = (int)p3[j].y + caja.y;
                        }
                        break;
                        case 3:
                        for(int j=0;j<4;j++)
                        {
                            (detPatts+((*cantDetectedPatts)-1))->esquinas[j].x = (int)p4[j].x + caja.x;
                            (detPatts+((*cantDetectedPatts)-1))->esquinas[j].y = (int)p4[j].y + caja.y;
                        }
                        break;
                    }
                }

//                cvShowImage((QString("Fixed #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(i)).toAscii().data(),fixedSquare);
//                cvShowImage((QString("Conto #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(i)).toAscii().data(),contoSquare);
//                cvShowImage((QString("Patt1 #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(i)).toAscii().data(),pattSquare[0]);
//                cvShowImage((QString("Patt2 #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(i)).toAscii().data(),pattSquare[1]);
//                cvShowImage((QString("Patt3 #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(i)).toAscii().data(),pattSquare[2]);
//                cvShowImage((QString("Patt4 #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(i)).toAscii().data(),pattSquare[3]);

//                cvShowImage((QString("Patt1 #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(0)).toAscii().data(),patt1Square);
//                cvShowImage((QString("Patt2 #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(0)).toAscii().data(),patt2Square);
//                cvShowImage((QString("Patt3 #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(0)).toAscii().data(),patt3Square);
//                cvShowImage((QString("Patt4 #") + QString::number(imgNumber) + QString(", Contorno #") + QString::number(0)).toAscii().data(),patt4Square);

                cvReleaseMat(&warpMatrix[0]);
                cvReleaseMat(&warpMatrix[1]);
                cvReleaseMat(&warpMatrix[2]);
                cvReleaseMat(&warpMatrix[3]);

                cvReleaseImage(&fixedSquare);
                cvReleaseImage(&contoSquare);
//                cvReleaseImage(&pattSquare[0]);
//                cvReleaseImage(&pattSquare[1]);
//                cvReleaseImage(&pattSquare[2]);
//                cvReleaseImage(&pattSquare[3]);
                i++;
            }


        }
    }
    cvReleaseMemStorage(&storage);
}

void MainWindow::renProcess(CvArPattFound *detPatts, int cantDetectedPatts, IplImage * camara)
{

    GLdouble m[20][16];
    int indDet[20];

    CvMat *intrinsic_matrix = cvCreateMat( 3, 3, CV_32FC1 );
    CvMat *object_points    = cvCreateMat( 4, 3, CV_32FC1 );
    CvMat *point_counts     = cvCreateMat( 1, 1, CV_32SC1 );

    cvZero(object_points);
    cvSet(point_counts,cvScalar(4));

    for(int i=0;i<4;i++)
    {
        cvSetReal2D(object_points,i,0,((i==0 || i==3)? 50.0f : -50.0f));
        cvSetReal2D(object_points,i,1,((i==0 || i==1)? 50.0f : -50.0f));
        cvSetReal2D(object_points,i,2,0.0f);
    }

    double fu = 794.54;
    double fv = 774.31;
    double cx = 320.0;
    double cy = 240.0;

    cvZero(intrinsic_matrix);
    cvSetReal2D(intrinsic_matrix,0,0,fu);
    cvSetReal2D(intrinsic_matrix,1,1,fv);
    cvSetReal2D(intrinsic_matrix,0,2,cx);
    cvSetReal2D(intrinsic_matrix,1,2,cy);
    cvSetReal2D(intrinsic_matrix,2,2, 1);

    GLdouble p[16];
    getProjectionMatrix(intrinsic_matrix,0.1,100.0,p);

    for(int i=0;i<cantDetectedPatts;i++)
    {
        CvMat *image_points     = cvCreateMat( 4, 2, CV_32FC1 );
        CvMat *rot_vec          = cvCreateMat( 1, 3, CV_32FC1 );
        CvMat *rot_mat          = cvCreateMat( 3, 3, CV_32FC1 );
        CvMat *tra_vec          = cvCreateMat( 1, 3, CV_32FC1 );

        indDet[i] = (detPatts+i)->numPatt;

        for (int j=0;j<4;j++)
        {
            cvSetReal2D(image_points,j,0,(double)(detPatts+i)->esquinas[j].x);
            cvSetReal2D(image_points,j,1,(double)(detPatts+i)->esquinas[j].y);
        }

        cvFindExtrinsicCameraParams2(object_points, image_points, intrinsic_matrix, NULL, rot_vec,tra_vec);

        cvSetReal2D(rot_vec,0,0,-cvGetReal2D(rot_vec,0,0));
        cvSetReal2D(rot_vec,0,1,-cvGetReal2D(rot_vec,0,1));
        cvRodrigues2(rot_vec,rot_mat);

        GLdouble mAct[16];
        double scaleFactor = 0.025 * zoom / 2.5096;
        getModelviewMatrix(rot_mat,tra_vec,scaleFactor,mAct);

        for(int j=0;j<16;j++)
        {
            m[i][j] = mAct[j];
        }
        cvReleaseMat(&image_points);
        cvReleaseMat(&rot_vec);
        cvReleaseMat(&rot_mat);
        cvReleaseMat(&tra_vec);
    }
    imActualGLWidget->setImage(camara);
    imActualGLWidget->setModelProject(p,m,indDet,cantDetectedPatts);

    cvReleaseMat(&intrinsic_matrix);
    cvReleaseMat(&object_points);
    cvReleaseMat(&point_counts);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    camara = cvQueryFrame(camCapture);
    grises = cvCreateImage(cvGetSize(camara), IPL_DEPTH_8U,1);

    camara->origin = CV_ORIGIN_BL;
    cvFlip(camara,camara,0);
    grises->origin = CV_ORIGIN_BL;

    CvArPattFound detPatts[20];
    int cantDetectedPatts = 0;

    preProcess(camara,grises,0);
    posProcess(grises,camara,&cantDetectedPatts,detPatts);

//    cvShowImage("Gris",grises);

//    if(cantDetectedPatts == 0)
//    {
//        preProcess(camara,grises,1);
//        posProcess(grises,camara,&cantDetectedPatts,detPatts);
//    }

    cvCvtColor(camara,camara,CV_BGR2RGB);
    renProcess(detPatts,cantDetectedPatts,camara);
    cvReleaseImage(&grises);
}

void MainWindow::readCam()
{
    camCapture = cvCaptureFromCAM( 0 );
    timerId = startTimer(40);
}

void MainWindow::stopCam()
{
    killTimer(timerId);
    cvReleaseCapture(&camCapture);
}

void MainWindow::on_actionStart_triggered()
{
    readCam();
}

void MainWindow::on_actionExit_triggered()
{
    this->~MainWindow();
}

void MainWindow::on_actionIncrease_Zoom_triggered()
{
    zoom -=0.5;
}

void MainWindow::on_actionDecrease_Zoom_triggered()
{
    zoom +=0.5;
}
