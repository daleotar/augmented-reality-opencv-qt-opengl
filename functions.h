#include "mainwindow.h"
#include <stdio.h>
#define PI 3.1415926535897932384626433832795
const int rotSequences[16][4] =
{
    { 0, 3,15,12},
    { 1, 7,14, 8},
    { 2,11,13, 4},
    { 3,15,12, 0},
    { 4, 2,11,13},
    { 5, 6,10, 9},
    { 6,10, 9, 5},
    { 7,14, 8, 1},
    { 8, 1, 7,14},
    { 9, 5, 6,10},
    {10, 9, 5, 6},
    {11,13, 4, 2},
    {12, 0, 3,15},
    {13, 4, 2,11},
    {14, 8, 1, 7},
    {15,12, 0, 3}
};

CvPoint cvGetImageCenter(IplImage *img)
{
    double ancho = (double)img->width;
    double alto  = (double)img->height;

    ancho /= 2.0;
    alto  /= 2.0;

    return cvPoint(cvRound(ancho),cvRound(alto));
}

double cvPointDistance(CvPoint puntoA, CvPoint puntoB)
{
    return sqrt( pow(puntoA.x - puntoB.x,2) + pow(puntoA.y - puntoB.y,2) );
}

int cvGetHighSide(CvPoint *fourCorners)
{
    int maxSide = 0;
    int actSide = 0;
    for(int i=0;i<4;i++)
    {
        if(i==3)
        {
            actSide = (int)sqrt( pow(fourCorners[i].x-fourCorners[0].x,2) + pow(fourCorners[i].y-fourCorners[0].y,2) );
        }
        else
        {
            actSide = (int)sqrt( pow(fourCorners[i].x-fourCorners[i+1].x,2) + pow(fourCorners[i].y-fourCorners[i+1].y,2) );
        }
        if (actSide > maxSide)
            maxSide = actSide;
    }
    return maxSide;
}

IplImage* muestrahist(CvHistogram *hist,int hist_size){
        int i,bin_w,var=0,P;
        char *tex;char t1[33];
        IplImage* hist_image;
        float max_value = 0;
        CvScalar Color;
        cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
        hist_image = cvCreateImage(cvSize(800,400), 8, 3);
        cvSet(hist_image,CV_RGB(255, 0, 255));

        cvScale( hist->bins, hist->bins, ((double)hist_image->height)/max_value, 0 );
                cvSet( hist_image, cvScalarAll(255), 0 );
                   bin_w = cvRound((double)hist_image->width/hist_size);
                   for( i = 0; i < hist_size; i++ ){
                       P=i%2;
                       if (P==1)
                       { Color=CV_RGB(0, 0, 255);}
                       else
                       {Color=CV_RGB(0, 255, 0);}

                       cvRectangle( hist_image, cvPoint(i*bin_w,hist_image->height-50),
                                    cvPoint((i+1)*bin_w, hist_image->height-50 - cvRound(cvGetReal1D(hist->bins,i)))
                                    ,Color, -1, 8,0);

                       if(var==i){
                       var=var+50;
                       itoa(i,t1,10);
                       tex=t1;
                       CvFont letra;double hScale=1;double vScale=1;int    lineWidth=1;
                       cvInitFont(&letra,CV_FONT_HERSHEY_COMPLEX_SMALL|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
                       cvPutText(hist_image,tex,cvPoint(i*bin_w,hist_image->height-20),&letra,CV_RGB(0, 0, 255));
                       Color=CV_RGB(0, 0, 255);
                       }
                       else
                       {
                           Color=CV_RGB(i, i, i);

                       }


                       cvRectangle( hist_image, cvPoint(i*bin_w,hist_image->height-49),
                                    cvPoint((i+1)*bin_w, hist_image->height-40)
                                    ,Color, -1, 8,0);



                   }

                   return(hist_image);
}

CvHistogram* histograma(IplImage* img1, int *umbral, char *nombre = "Histograma")
{
    int lowerInd=0, higherInd=256;
    float minValue, maxValue;
    float range_0[]={0,256};
    int hist_size=256;
    float* ranges[] = { range_0 };
//    IplImage *Histograma;
    CvHistogram *hist;
    hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
    cvCalcHist( &img1, hist, 0, 0 );

    (*(cvGetHistValue_1D(hist,0))) = 0;
    (*(cvGetHistValue_1D(hist,255))) = 0;

    cvGetMinMaxHistValue(hist, &minValue, &maxValue);
    cvThreshHist(hist,maxValue*0.10);

    for (int i=0;i<256;i++)
    {
        if((*(cvGetHistValue_1D(hist,i)))!=0)
        {
            lowerInd = i;
        }
    }

    for (int i=255;i>=0;i--)
    {
        if((*(cvGetHistValue_1D(hist,i)))!=0)
        {
            higherInd = i;
        }
    }

    *umbral = (lowerInd + higherInd)/2;

//    Histograma=muestrahist(hist,hist_size);
//    cvShowImage(nombre,Histograma);
    return hist;
    cvReleaseHist(&hist);
//    cvReleaseImage(&Histograma);

}

void cvGetHistoThresh(IplImage *img, int *umbral)
{
    int lowerInd=0, higherInd=256;
    float minValue, maxValue;
    float range_0[]={0,256};
    int hist_size=256;
    float* ranges[] = { range_0 };
    CvHistogram *hist;
    hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
    cvCalcHist( &img, hist, 0, 0 );

    (*(cvGetHistValue_1D(hist,0))) = 0;
    (*(cvGetHistValue_1D(hist,255))) = 0;

    cvGetMinMaxHistValue(hist, &minValue, &maxValue);
    cvThreshHist(hist,maxValue*0.10);

    for (int i=0;i<256;i++)
    {
        if((*(cvGetHistValue_1D(hist,i)))!=0)
        {
            lowerInd = i;
        }
    }

    for (int i=255;i>=0;i--)
    {
        if((*(cvGetHistValue_1D(hist,i)))!=0)
        {
            higherInd = i;
        }
    }

    *umbral = (lowerInd + higherInd)/2;
    cvReleaseHist(&hist);
}

int cvFlipY(int yPoint, int imgHeight)
{
    return imgHeight - yPoint;
}

double cvAbsVal(double val)
{
    return (val < 0.0) ? val*(-1) : val;
}

double cvRad2Deg (double angle, bool scale=true)
{
  double ratio = 180.0 / PI;
  double angDeg = angle * ratio;
  if (scale)
  return ((angDeg < 0.0)? angDeg+360.0 : angDeg );
  else
  return angDeg;
}

double cvDeg2Rad (double angle, bool scale=true)
{
  double ratio = PI / 180.0;
  double angRad = angle * ratio;
  if (scale)
  return ((angRad < 0.0)? angRad + (2*PI) : angRad );
  else
  return angRad;
}

void cvFindTrueCorners(CvPoint sortedPoints[], int cantCorners, CvPoint *trueCorners, int *cantTrueCorners, int angle=145)
{
    int pivote=0, pAnt=0, pSig=0, maxAng=0, minAng=0;
    double alfa1=0.0, alfa2=0.0;

    for (int i=0;i<cantCorners;i++)
    {
        pivote=i;
        if (i==0)
        {
            pAnt=cantCorners-1;
            pSig=1;
        }
        else if(i==(cantCorners-1))
        {
            pAnt=i-1;
            pSig=0;
        }
        else
        {
            pAnt=i-1;
            pSig=i+1;
        }
        alfa1 = cvRad2Deg(atan2((double) (sortedPoints[pAnt].y - sortedPoints[pivote].y) , (double)(sortedPoints[pAnt].x - sortedPoints[pivote].x)));
        alfa2 = cvRad2Deg(atan2((double) (sortedPoints[pSig].y - sortedPoints[pivote].y) , (double)(sortedPoints[pSig].x - sortedPoints[pivote].x)));
        maxAng = abs((int)(alfa2 - alfa1));
        minAng = 360 - maxAng;
//        qDebug() << "Angulos:" << maxAng << minAng << "Pivote:" << pivote << "PAnt:" << pAnt << "PSig:" << pSig;
        if ((minAng <= angle) || (maxAng <= angle))
        {
            (*(trueCorners+*cantTrueCorners)).x = sortedPoints[i].x;
            (*(trueCorners+*cantTrueCorners)).y = sortedPoints[i].y;
            (*cantTrueCorners)++;
        }

    }
}

void cvCornerArrangeOrder(CvPoint points[], int count, CvPoint centro, CvPoint *sortedCorners)
{
    int angulos[count];
    int angulosInd[count];

    for (int i=0;i<count;i++)
    {
        angulos[i] = (int)(cvRad2Deg(atan2((double) (points[i].y - (float)(centro.y)) , (double)(points[i].x - (float)(centro.x)) )));
        angulosInd[i] = angulos[i];
    }

    std::sort(angulos, angulos+count);

    for (int i=0;i<count;i++)
    {
        for (int j=0;j<count;j++)
        {
            if(angulos[i]==angulosInd[j])
            {
                (*(sortedCorners + i)).x = (int)(points[j].x);
                (*(sortedCorners + i)).y = (int)(points[j].y);
                break;
            }
        }
    }
}

int cvCheckRepeat(CvSeq* contour, int maxCount=10)
{
    int count=0;
    for (int i=0;i<contour->total-1;i++)
    {
        CvPoint *puntoCheck = (CvPoint *) cvGetSeqElem(contour,i);
        for (int j=i+1;j<contour->total-1;j++)
        {
            CvPoint *puntoCompare = (CvPoint *) cvGetSeqElem(contour,j);
            if (puntoCheck->x == puntoCompare->x && puntoCheck->y == puntoCompare->y)
            {
                count++;
                if (count==maxCount)
                {
                break;
                }
            }

        }
    }
    return count;

}

void cvFindChainCorners (CvSeq* contour, CvPoint *corners, int *cornerCount, int iterations=2, int minLenght=5)
{
    int i;
    int chainCode;
    QString chainString = "";
    for (i=0;i<contour->total;i++) // Conversion de CvPoint a Freeman Code
    {
        CvPoint *puntoAct = (CvPoint *) cvGetSeqElem(contour,i);
        CvPoint *puntoSig;
        if (i!=contour->total-1)
        {
            puntoSig = (CvPoint *) cvGetSeqElem(contour,i+1);
        }
        else
        {
            puntoSig = (CvPoint *) cvGetSeqElem(contour,0);
        }
        if (puntoSig->x > puntoAct->x)
        {
            if (puntoSig->y < puntoAct->y)
            {
                chainCode = 7;
            }
            if ((puntoSig->y) > (puntoAct->y))
            {
                chainCode = 1;
            }
            if (puntoSig->y == puntoAct->y)
            {
                chainCode = 0;
            }
        }
        if (puntoSig->x < puntoAct->x)
        {
            if (puntoSig->y < puntoAct->y)
            {
                chainCode = 5;
            }
            if (puntoSig->y > puntoAct->y)
            {
                chainCode = 3;
            }
            if (puntoSig->y == puntoAct->y)
            {
                chainCode = 4;
            }
        }
        if (puntoSig->x == puntoAct->x)
        {
            if (puntoSig->y < puntoAct->y)
            {
                chainCode = 6;
            }
            if (puntoSig->y > puntoAct->y)
            {
                chainCode = 2;
            }
            if (puntoSig->y == puntoAct->y)
            {
                chainCode = 8;
            }
        }
        chainString += QString::number(chainCode);
    }

    if (chainString.length() > 10 )
    {

        for (i=0; i<iterations; i++) // Correccion de líneas simples
        {
            chainString.replace(QString("071"),QString("000"));
            chainString.replace(QString("017"),QString("000"));
            chainString.replace(QString("171"),QString("100"));
            chainString.replace(QString("717"),QString("700"));
            chainString.replace(QString("313"),QString("322"));
            chainString.replace(QString("131"),QString("122"));
            chainString.replace(QString("757"),QString("766"));
            chainString.replace(QString("575"),QString("566"));
            chainString.replace(QString("231"),QString("222"));
            chainString.replace(QString("213"),QString("222"));
            chainString.replace(QString("453"),QString("444"));
            chainString.replace(QString("435"),QString("444"));
            chainString.replace(QString("535"),QString("544"));
            chainString.replace(QString("353"),QString("344"));
            chainString.replace(QString("657"),QString("666"));
            chainString.replace(QString("675"),QString("666"));
            chainString.replace(QString("120"),QString("11"));
            chainString.replace(QString("102"),QString("11"));
            chainString.replace(QString("342"),QString("33"));
            chainString.replace(QString("324"),QString("33"));
            chainString.replace(QString("564"),QString("55"));
            chainString.replace(QString("546"),QString("55"));
            chainString.replace(QString("706"),QString("77"));
            chainString.replace(QString("760"),QString("77"));
        }
        int totalBC = chainString.length();
        int chainBC[totalBC+4];
        int chainDC[totalBC];
        int nonZero[totalBC/2];
        int cantNonZero = 0;

        CvPoint fixed[totalBC];

        fixed[0].x = (((CvPoint *) cvGetSeqElem(contour,0))->x);
        fixed[0].y = (((CvPoint *) cvGetSeqElem(contour,0))->y);

        chainBC[0] = chainString.at(totalBC-2).digitValue();
        chainBC[1] = chainString.at(totalBC-1).digitValue();
        chainBC[totalBC+2] = chainString.at(0).digitValue();
        chainBC[totalBC+3] = chainString.at(1).digitValue();

        for (i=0;i<totalBC;i++)
        {
            chainBC[i+2] = chainString.at(i).digitValue();
        }

        for (i=0;i<totalBC;i++)
        {
            int a=chainBC[i];
            int b=chainBC[i+1];
            int act=chainBC[i+2];
            int c=chainBC[i+3];
            int d=chainBC[i+4];

            if(a==b && c==d && b==c && act!=a)
            chainBC[i+2] = c;

            if(a==c && b==c and c!=act)
            chainBC[i+2] = c;

            if(c==b && d==b and b!=act)
            chainBC[i+2] = b;

            if(a==d && b==d and d!=act)
            chainBC[i+2] = d;

            if(c==a && d==a and a!=act)
            chainBC[i+2] = a;

            chainDC[i] = abs(chainBC[i+1] - chainBC[i+2]);
    //        chainString.at(i-2) = (QString::number(chainBC[i])).at(0);

            if (i<=totalBC-2)
            {
                switch (chainString.at(i).digitValue())
                {
                case 0:
                fixed[i+1].x = (fixed[i].x)+1;
                fixed[i+1].y = (fixed[i].y)+0;
                break;
                case 1:
                fixed[i+1].x = (fixed[i].x)+1;
                fixed[i+1].y = (fixed[i].y)+1;
                break;
                case 2:
                fixed[i+1].x = (fixed[i].x)+0;
                fixed[i+1].y = (fixed[i].y)+1;
                break;
                case 3:
                fixed[i+1].x = (fixed[i].x)-1;
                fixed[i+1].y = (fixed[i].y)+1;
                break;
                case 4:
                fixed[i+1].x = (fixed[i].x)-1;
                fixed[i+1].y = (fixed[i].y)+0;
                break;
                case 5:
                fixed[i+1].x = (fixed[i].x)-1;
                fixed[i+1].y = (fixed[i].y)-1;
                break;
                case 6:
                fixed[i+1].x = (fixed[i].x)+0;
                fixed[i+1].y = (fixed[i].y)-1;
                break;
                case 7:
                fixed[i+1].x = (fixed[i].x)+1;
                fixed[i+1].y = (fixed[i].y)-1;
                break;
                }
            }

            if(chainDC[i] != 0)
            {
                nonZero[cantNonZero] = i;
                cantNonZero++;
            }

        }
        int trueCorners[cantNonZero];
        int cantTrueCorners = 0;
        int dif = 0;
        int previousNonZero = nonZero[cantNonZero-1];
        for (i=0;i<cantNonZero;i++)
        {
            if (previousNonZero > nonZero[i])
            {
                dif = totalBC-1-previousNonZero+nonZero[i]+1;
            }
            else
            {
                dif = nonZero[i] - previousNonZero;
            }

            if (dif>minLenght)
            {
                previousNonZero = nonZero[i];
                trueCorners[cantTrueCorners] = nonZero[i];
                cantTrueCorners++;
            }
        }
        *cornerCount = cantTrueCorners;
        for (i=0;i<cantTrueCorners;i++)
        {
            (*corners).x = fixed[trueCorners[i]].x;
            (*corners).y = fixed[trueCorners[i]].y;
            corners++;
        }
    }

//    chainString.clear();
//    chainString.detach();
}

void cvPattExtractChar(IplImage *imagen, CvArPattern *patron)
{
    double squareMalla = (double)(imagen->width)/5.0;
    patron->tam = cvGetSize(imagen);
    CvPoint cornerMarkers[16];

    int maxLevel = 1;
    CvSeq* contours = 0;
    CvSeq* contourAct = 0;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvTreeNodeIterator iterator;

    cvFindContours(imagen, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP ,CV_CHAIN_APPROX_NONE);

    if(contours != 0)
    cvInitTreeNodeIterator( &iterator, contours, maxLevel);

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            cornerMarkers[(i*4)+j] = cvPoint((int)(squareMalla*(j+1)),((int)squareMalla*(i+1)));
        }
    }

    while( (contourAct = (CvSeq*)cvNextTreeNode(&iterator))!= 0 )
    {
        CvMoments centro;
        cvMoments(contourAct,&centro,1);

        double m10=cvGetSpatialMoment(&centro,1,0);
        double m00=cvGetSpatialMoment(&centro,0,0);
        double m01=cvGetSpatialMoment(&centro,0,1);

        patron->centroid = cvPoint((int)(m10/m00),(int)(m01/m00));
//        cvCircle(patron->img,patron->centroid,5,cvScalar(175),2);
        patron->periRelation = (double)(contourAct->total)/(double)(2*(imagen->width+imagen->height));
        patron->areaRelation = cvContourArea(contourAct)/(imagen->width*imagen->height);

        const int MAX_CORNERS = 50;
        CvPoint corners[MAX_CORNERS];
        int cornerCount = 0;

        cvFindChainCorners(contourAct,corners,&cornerCount);
        for(int i=0;i<16;i++)
        {
            patron->cornerAt[i] = false;
            for(int k=0;k<cornerCount;k++)
            {
//                cvCircle(patron->img,corners[k],5,cvScalar(175),2);
                if(cvPointDistance(corners[k],cornerMarkers[i]) < (squareMalla/3))
                {
                    patron->cornerAt[i] = true;
                }
            }
//            qDebug() << "Esquina #" << i << patron->cornerAt[i];
        }
//        qDebug() << "Relacion de área     :" << patron->areaRelation;
//        qDebug() << "Relacion de perímetro:" << patron->periRelation;
//        qDebug() << "Centroide            : x=" << patron->centroid.x << ", y=" << patron->centroid.y;
//        qDebug() << "Esquinas             :" << patron->cantCorners;
//        qDebug() << "";
    }
    cvReleaseImage(&imagen);
    cvReleaseMemStorage(&storage);
}

CvPoint cvTransformPoint(CvPoint punto, CvMat *warpMatrix)
{
    double x = (double) punto.x;
    double y = (double) punto.y;

    double w = x*cvGetReal2D(warpMatrix,2,0) + y*cvGetReal2D(warpMatrix,2,1) + cvGetReal2D(warpMatrix,2,2);

    w = 1/w;

    double xR = (x*cvGetReal2D(warpMatrix,0,0) + y*cvGetReal2D(warpMatrix,0,1) + cvGetReal2D(warpMatrix,0,2))*w;
    double yR = (x*cvGetReal2D(warpMatrix,1,0) + y*cvGetReal2D(warpMatrix,1,1) + cvGetReal2D(warpMatrix,1,2))*w;

    if (x<0 || y<0)
    {
        return cvPoint(0,0);
    }
    else
    {
        return cvPoint((int)xR,(int)yR);
    }


}

void cvArPattRotCorners(CvArPattern *patron, int angulo)
{
    bool cornerAt[16];

    for (int i=0;i<16;i++)
    {
        cornerAt[rotSequences[i][angulo]] = (*patron).cornerAt[i];
    }
    for (int i=0;i<16;i++)
    {
        (*patron).cornerAt[i] = cornerAt[i];
    }
}

CvPoint cvRotatePoint(CvPoint punto, CvPoint2D32f pivote, double angulo)
{
    double difX = (double)(punto.x) - (double)(pivote.x);
    double difY = (double)(punto.y) - (double)(pivote.y);

    double newX = difX*cos(cvDeg2Rad(angulo)) - difY*sin(cvDeg2Rad(angulo));
    double newY = difX*sin(cvDeg2Rad(angulo)) + difY*cos(cvDeg2Rad(angulo));

    newX += (double)(pivote.x);
    newY += (double)(pivote.y);

    if (newX<0 || newY<0)
    {
        return cvPoint(0,0);
    }
    else
    {
        return cvPoint(cvRound(newX),cvRound(newY));
    }



}

void cvMatchArPatt(CvArPattern pattIdeal, CvArPattern pattReal, double squareMalla, bool *result)
{
    double difAreaRel  = cvAbsVal(pattIdeal.areaRelation - pattReal.areaRelation)*100.0;
    double difPeriRel  = cvAbsVal(pattIdeal.periRelation - pattReal.periRelation)*100.0;
    double distCentro  = cvPointDistance(pattIdeal.centroid, pattReal.centroid);
    int    cornerMatch = 0;

    for(int i=0;i<16;i++)
    {
        if (pattIdeal.cornerAt[i] == pattReal.cornerAt[i])
        cornerMatch++;
    }

    int cantCond = 3;
    int condiciones[cantCond];
    int cantCondCump = 0;

//    condiciones[0] = (difAreaRel<5.0)? 1 : 0 ;
    condiciones[0] = (difPeriRel<6.5)? 1 : 0 ;
    condiciones[1] = (distCentro<=squareMalla*5.0/12.0)? 1 : 0;
    condiciones[2] = (cornerMatch>=15)? 1 : 0;

//    qDebug() << condiciones[0] << condiciones[1] << condiciones[2] << condiciones[3] << condiciones[4];

    for (int i=0;i<cantCond;i++)
    {
        cantCondCump +=condiciones[i];
    }

    *result = (cantCondCump==cantCond)? true : false;
}

void cvGenericPattExtractChar(IplImage *img, CvArPattern *patron, CvSeq *contorno,CvMat *warpMatrix, double squareMalla)
{
    CvPoint cornerMarkers[16];

    int maxLevel = 1;
    CvSeq* contours = 0;
    CvSeq* contourAct = 0;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvTreeNodeIterator iterator;

//    IplImage *patronShift = cvCreateImage(patron->tam,8,1);
//    cvWarpPerspective(img,patronShift,warpMatrix);


    cvFindContours(img, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP ,CV_CHAIN_APPROX_NONE);

    if(contours != 0)
    cvInitTreeNodeIterator( &iterator, contours, maxLevel);

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            cornerMarkers[(i*4)+j] = cvPoint((int)(squareMalla*(j+1)),((int)squareMalla*(i+1)));
        }
    }

    while( (contourAct = (CvSeq*)cvNextTreeNode(&iterator))!= 0 )
    {
        double areaContComp = cvContourArea(contorno);
        double areaContActu = cvContourArea(contourAct);

        if(areaContActu < (areaContComp*0.9) && areaContActu >= areaContComp*0.028)
        {
            CvMoments centro;
            cvMoments(contourAct,&centro,1);

            double m10=cvGetSpatialMoment(&centro,1,0);
            double m00=cvGetSpatialMoment(&centro,0,0);
            double m01=cvGetSpatialMoment(&centro,0,1);

            patron->centroid = cvTransformPoint(cvPoint((int)(m10/m00),(int)(m01/m00)),warpMatrix);
//            cvCircle(imagenPatt,patron->centroid,5,cvScalar(175),2);
            patron->periRelation = (double)(contourAct->total)/(double)(contorno->total);
            patron->areaRelation = cvContourArea(contourAct)/cvContourArea(contorno);

            const int MAX_CORNERS = 50;
            CvPoint corners[MAX_CORNERS];
            int cornerCount = 0;

            cvFindChainCorners(contourAct,corners,&cornerCount);



            for(int i=0;i<16;i++)
            {
                patron->cornerAt[i] = false;
                for(int k=0;k<cornerCount;k++)
                {
                    cvCircle(img,corners[k],5,cvScalar(175),2);
//                    cvCircle(patronShift,cvTransformPoint(corners[k],warpMatrix),5,cvScalar(175),2);
                    if(cvPointDistance(cvTransformPoint(corners[k],warpMatrix),cornerMarkers[i]) < (squareMalla*5.0/12.0))
                    {
                        patron->cornerAt[i] = true;
                    }
                }
//                qDebug() << "Esquina #" << i << actGenePatt->cornerAt[i];
            }
//            qDebug() << "Relacion de área     :" << actGenePatt->areaRelation;
//            qDebug() << "Relacion de perímetro:" << actGenePatt->periRelation;
//            qDebug() << "Centroide            : x=" << actGenePatt->centroid.x << ", y=" << actGenePatt->centroid.y;
//            qDebug() << "Esquinas             :" << actGenePatt->cantCorners;
//            qDebug() << "";
        }
    }
//    cvShowImage("Shift",patronShift);
//    cvReleaseImage(&patronShift);
    cvReleaseMemStorage(&storage);
}

void cvInitArPattern(CvArPattern *patron)
{
    patron->areaRelation = 0;
    patron->centroid = cvPoint(0,0);
    patron->periRelation = 0;
    patron->tam = cvSize(0,0);

    for (int i=0; i<16; i++)
    {
        patron->cornerAt[i] = false;
    }
}

static double norm( double a, double b, double c )
{
    return( sqrt( a*a + b*b + c*c ) );
}

static double dot( double a1, double a2, double a3, double b1, double b2, double b3 )
{
    return( a1 * b1 + a2 * b2 + a3 * b3 );
}

void reshapeIntrinsic(CvMat *intrinsic, double camParam[3][4])
{
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
        camParam[i][j] = cvGetReal2D(intrinsic,i,j);
        }
    }
    camParam[0][3] = 0;
    camParam[1][3] = 0;
    camParam[2][3] = 0;
}

int  arParamDecompMat( double source[3][4], double cpara[3][4], double trans[3][4] )
{
    int       r, c;
    double    Cpara[3][4];
    double    rem1, rem2, rem3;

    if( source[2][3] >= 0 ) {
        for( r = 0; r < 3; r++ ){
            for( c = 0; c < 4; c++ ){
                Cpara[r][c] = source[r][c];
            }
        }
    }
    else {
        for( r = 0; r < 3; r++ ){
            for( c = 0; c < 4; c++ ){
                Cpara[r][c] = -(source[r][c]);
            }
        }
    }

    for( r = 0; r < 3; r++ ){
        for( c = 0; c < 4; c++ ){
            cpara[r][c] = 0.0;
        }
    }
    cpara[2][2] = norm( Cpara[2][0], Cpara[2][1], Cpara[2][2] );
    trans[2][0] = Cpara[2][0] / cpara[2][2];
    trans[2][1] = Cpara[2][1] / cpara[2][2];
    trans[2][2] = Cpara[2][2] / cpara[2][2];
    trans[2][3] = Cpara[2][3] / cpara[2][2];

    cpara[1][2] = dot( trans[2][0], trans[2][1], trans[2][2],
                       Cpara[1][0], Cpara[1][1], Cpara[1][2] );
    rem1 = Cpara[1][0] - cpara[1][2] * trans[2][0];
    rem2 = Cpara[1][1] - cpara[1][2] * trans[2][1];
    rem3 = Cpara[1][2] - cpara[1][2] * trans[2][2];
    cpara[1][1] = norm( rem1, rem2, rem3 );
    trans[1][0] = rem1 / cpara[1][1];
    trans[1][1] = rem2 / cpara[1][1];
    trans[1][2] = rem3 / cpara[1][1];

    cpara[0][2] = dot( trans[2][0], trans[2][1], trans[2][2],
                       Cpara[0][0], Cpara[0][1], Cpara[0][2] );
    cpara[0][1] = dot( trans[1][0], trans[1][1], trans[1][2],
                       Cpara[0][0], Cpara[0][1], Cpara[0][2] );
    rem1 = Cpara[0][0] - cpara[0][1]*trans[1][0] - cpara[0][2]*trans[2][0];
    rem2 = Cpara[0][1] - cpara[0][1]*trans[1][1] - cpara[0][2]*trans[2][1];
    rem3 = Cpara[0][2] - cpara[0][1]*trans[1][2] - cpara[0][2]*trans[2][2];
    cpara[0][0] = norm( rem1, rem2, rem3 );
    trans[0][0] = rem1 / cpara[0][0];
    trans[0][1] = rem2 / cpara[0][0];
    trans[0][2] = rem3 / cpara[0][0];

    trans[1][3] = (Cpara[1][3] - cpara[1][2]*trans[2][3]) / cpara[1][1];
    trans[0][3] = (Cpara[0][3] - cpara[0][1]*trans[1][3]
                               - cpara[0][2]*trans[2][3]) / cpara[0][0];

    for( r = 0; r < 3; r++ ){
        for( c = 0; c < 3; c++ ){
            cpara[r][c] /= cpara[2][2];
        }
    }

    return 0;
}

void getProjectionMatrix(CvMat *intrinsic, const double focalmin, const double focalmax, GLdouble m_projection[16])
{
    double   camparam[3][4];
    double   icpara[3][4];
    double   trans[3][4];
    double   p[3][3], q[4][4];
    int      width, height;
    int      i, j;

    reshapeIntrinsic(intrinsic, camparam);

    width  = 640;
    height = 480;

    if (arParamDecompMat(camparam, icpara, trans) < 0) {
        printf("arglCameraFrustum(): arParamDecompMat() indicated parameter error.\n"); // Windows bug: when running multi-threaded, can't write to stderr!
        return;
    }
        for (i = 0; i < 4; i++) {
        icpara[1][i] = ((height - 1)*(icpara[2][i])) - icpara[1][i];
    }

    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            p[i][j] = icpara[i][j] / icpara[2][2];
        }
    }
    q[0][0] = (2.0 * p[0][0] / (width - 1));
    q[0][1] = (2.0 * p[0][1] / (width - 1));
    q[0][2] = -((2.0 * p[0][2] / (width - 1))  - 1.0);
    q[0][3] = 0.0;

    q[1][0] = 0.0;
    q[1][1] = -(2.0 * p[1][1] / (height - 1));
    q[1][2] = -((2.0 * p[1][2] / (height - 1)) - 1.0);
    q[1][3] = 0.0;

    q[2][0] = 0.0;
    q[2][1] = 0.0;
    q[2][2] = (focalmax + focalmin)/(focalmin - focalmax);
    q[2][3] = 2.0 * focalmax * focalmin / (focalmin - focalmax);

    q[3][0] = 0.0;
    q[3][1] = 0.0;
    q[3][2] = -1.0;
    q[3][3] = 0.0;

    for (i = 0; i < 4; i++) { // Row.
                // First 3 columns of the current row.
        for (j = 0; j < 3; j++) { // Column.
            m_projection[i + j*4] = q[i][0] * trans[0][j] +
                        q[i][1] * trans[1][j] +
                        q[i][2] * trans[2][j];
        }
                // Fourth column of the current row.
        m_projection[i + 3*4] = q[i][0] * trans[0][3] +
                                                                q[i][1] * trans[1][3] +
                                                                q[i][2] * trans[2][3] +
                                                                q[i][3];
    }
}

void getModelviewMatrix (CvMat *rot_mat,CvMat *tra_vec,double scaleFactor,GLdouble m_modelview[16])
{
    m_modelview[0 + 0*4] = cvGetReal2D(rot_mat,0,0); // R1C1
    m_modelview[0 + 1*4] = cvGetReal2D(rot_mat,0,1); // R1C2
    m_modelview[0 + 2*4] = cvGetReal2D(rot_mat,0,2);

    m_modelview[0 + 3*4] = cvGetReal2D(tra_vec,0,0);

    m_modelview[1 + 0*4] = cvGetReal2D(rot_mat,1,0); // R2
    m_modelview[1 + 1*4] = cvGetReal2D(rot_mat,1,1);
    m_modelview[1 + 2*4] = cvGetReal2D(rot_mat,1,2);

    m_modelview[1 + 3*4] =  cvGetReal2D(tra_vec,0,1);

    m_modelview[2 + 0*4] = cvGetReal2D(rot_mat,2,0); // R3
    m_modelview[2 + 1*4] = cvGetReal2D(rot_mat,2,1);
    m_modelview[2 + 2*4] = cvGetReal2D(rot_mat,2,2);

    m_modelview[2 + 3*4] = -cvGetReal2D(tra_vec,0,2);
    m_modelview[3 + 0*4] = 0.0;
    m_modelview[3 + 1*4] = 0.0;
    m_modelview[3 + 2*4] = 0.0;
    m_modelview[3 + 3*4] = 1.0;
    if (scaleFactor != 0.0) {
            m_modelview[12] *= scaleFactor;
            m_modelview[13] *= scaleFactor;
            m_modelview[14] *= scaleFactor;
    }
}

void cvAdjustGamma(IplImage *img, double gamma=2.8)
{
    IplImage *channel8 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
    IplImage *channel32 = cvCreateImage(cvGetSize(img),IPL_DEPTH_32F,1);
    double escala=1.0/255.0;
    for (int i=1;i<4;i++)
    {
        cvSetImageCOI(img,i);
        cvCopy(img,channel8);
        cvConvertScale(channel8,channel32,escala);
        cvPow(channel32,channel32,gamma);
        cvConvertScaleAbs(channel32,channel8,255.0);
        cvCopy(channel8,img);
    }
    cvSetImageCOI(img,0);
    cvReleaseImage(&channel8);
    cvReleaseImage(&channel32);
}

void cvGrayWorld(IplImage* img)
{
    IplImage *channel = cvCreateImage(cvGetSize(img),8,1);
    double avg[3];

    for (int i=1;i<4;i++)
    {
        cvSetImageCOI(img,i);
        avg[i-1] = cvAvg(img).val[0];
    }

    double avgGr = (avg[0] + avg[1] + avg[2])/3;

    for (int i=1;i<4;i++)
    {
        cvSetImageCOI(img,i);
        cvCopy(img,channel);
        cvConvertScaleAbs(channel,channel,avgGr/avg[i-1]);
        cvCopy(channel,img);
    }
    cvSetImageCOI(img,0);
    cvReleaseImage(&channel);
}
