#include "mesh.h"
#include "math.h"
#include <QDebug>

Mesh::Mesh(unsigned nbvertex, unsigned nbfaces, unsigned nbtexcords, unsigned nbnormals, unsigned nbmaterials)
{
    this->nbVertex=nbvertex;
    this->nbFaces=nbfaces;
    this->nbTexCords=nbtexcords;
    this->nbNormals=nbnormals;
    this->nbMaterials=nbmaterials;

    int vertexsize=nbVertex*3;
    int facesize=nbFaces*3;
    int vtsize=nbTexCords*3;
    int fnsize=facesize;

    if(nbVertex)
        this->vertices = new float[vertexsize+1];
    if(nbFaces)
        this->indices = new unsigned[facesize+1];
    if(nbTexCords)
        this->textcords = new float[vtsize];

    this->facenormals = new float[fnsize];
    this->vertexnormals = new float[vertexsize];
    this->facecenters = new float[fnsize];
    this->facematerials = new unsigned int[fnsize/3];
    this->materials = new float[nbmaterials*3];
    this->matalpha = new float[nbmaterials];
}
Mesh::~Mesh()
{
   delete [] vertices;
   delete [] indices;
   delete [] textcords;
   delete [] facecenters;
   delete [] facenormals;
   delete [] facematerials;
   delete [] vertexnormals;
   delete [] materials;
   delete [] matalpha;
}

Mesh::Mesh(const Mesh &copy)
{
    qDebug()<<"copiando";
}

void Mesh::generateFaceNormals()
{
    for(int i =0 ; i < nbFaces*3 ; i+=3)
    {
        float x0 = vertices[(3*indices[i])];
        float y0 = vertices[(3*indices[i])+1];
        float z0 = vertices[(3*indices[i])+2];

        float x1 = vertices[(3*indices[i+1])];
        float y1 = vertices[(3*indices[i+1])+1];
        float z1 = vertices[(3*indices[i+1])+2];

        float x2 = vertices[(3*indices[i+2])];
        float y2 = vertices[(3*indices[i+2])+1];
        float z2 = vertices[(3*indices[i+2])+2];

        float Px= x1-x0;
        float Py= y1-y0;
        float Pz= z1-z0;
        float Qx= x2-x1;
        float Qy= y2-y1;
        float Qz= z2-z1;

        float nx = Py*Qz - Pz*Qy;
        float ny = Pz*Qx - Px*Qz;
        float nz = Px*Qy - Py*Qx;

        float len = (float)(sqrt((nx * nx) + (ny * ny) + (nz * nz)));

        nx=nx/len;
        ny=ny/len;
        nz=nz/len;

        facenormals[i]  =nx;
        facenormals[i+1]=ny;
        facenormals[i+2]=nz;

        facecenters[i] =   (x0+x1+x2)/3;
        facecenters[i+1] = (y0+y1+y2)/3;
        facecenters[i+2] = (z0+z1+z2)/3;

    }
    /*for(int j =0 ; j < nbVertex*3 ; j+=3)
    {

    }*/


}

void Mesh::generateVertexNormals()
{
    float cosAngle = 0.95;
//    qDebug() << "Hay " << nbVertex << "Vertices";
    for(int i=0;i<nbVertex*3;i+=3)
    {
        vertexnormals[i]   = 0;
        vertexnormals[i+1] = 0;
        vertexnormals[i+2] = 0;
        int maxAdjFaces = 50;
        int numAdjFaces = 0;
        int adjFaces[maxAdjFaces];
        for(int j=0 ; j<nbFaces*3 ; j+=3)
        {
            if(indices[j]==i/3 || indices[j+1]==i/3 || indices[j+2]==i/3)
            {
                adjFaces[numAdjFaces] = j/3;
                numAdjFaces++;
            }
        }
//        qDebug() << "El vertice #" << i/3 << " tiene " << numAdjFaces << " caras adyacentes";

        if (numAdjFaces !=0)
        {
            float faceNormalX0 = facenormals[3*adjFaces[0]];
            float faceNormalY0 = facenormals[3*adjFaces[0]+1];
            float faceNormalZ0 = facenormals[3*adjFaces[0]+2];

            float faceNormalX;
            float faceNormalY;
            float faceNormalZ;

            int numFacesTaken = 0;

            for (int j=0; j<numAdjFaces; j++)
            {
                faceNormalX = facenormals[3*adjFaces[j]];
                faceNormalY = facenormals[3*adjFaces[j]+1];
                faceNormalZ = facenormals[3*adjFaces[j]+2];

//                if ((faceNormalX*faceNormalX0)+(faceNormalY*faceNormalY0)+(faceNormalZ*faceNormalZ0) > cosAngle)
//                {

                vertexnormals[i]   += faceNormalX;
                vertexnormals[i+1] += faceNormalY;
                vertexnormals[i+2] += faceNormalZ;

                numFacesTaken++;
//                }
            }

            vertexnormals[i]   /= numFacesTaken;
            vertexnormals[i+1] /= numFacesTaken;
            vertexnormals[i+2] /= numFacesTaken;

            float x = vertexnormals[i];
            float y = vertexnormals[i+1];
            float z = vertexnormals[i+2];

            float len = (float)(sqrt((x * x) + (y * y) + (z * z)));

            x=x/len;
            y=y/len;
            z=z/len;
            vertexnormals[i]=x;
            vertexnormals[i+1]=y;
            vertexnormals[i+2]=z;

        }

    }
}

void Mesh::separateFaces()
{
    faces = new Face[nbFaces];
    int idx = 0;
    for(int i=0;i<nbFaces*3;i+=3)
    {
        Face face;
        face.vertex = new Vertex[3];
        face.normals = new Vertex[3];
        face.vertex[0].points[0] = vertices[(3*indices[i])];
        face.vertex[0].points[1] = vertices[(3*indices[i])+1];
        face.vertex[0].points[2] = vertices[(3*indices[i])+2];

        face.vertex[1].points[0] = vertices[(3*indices[i+1])];
        face.vertex[1].points[1] = vertices[(3*indices[i+1])+1];
        face.vertex[1].points[2] = vertices[(3*indices[i+1])+2];

        face.vertex[2].points[0] = vertices[(3*indices[i+2])];
        face.vertex[2].points[1] = vertices[(3*indices[i+2])+1];
        face.vertex[2].points[2] = vertices[(3*indices[i+2])+2];

        face.normals[0].points[0] = vertexnormals[(3*indices[i])];
        face.normals[0].points[1] = vertexnormals[(3*indices[i])+1];
        face.normals[0].points[2] = vertexnormals[(3*indices[i])+2];

        face.normals[1].points[0] = vertexnormals[(3*indices[i+1])];
        face.normals[1].points[1] = vertexnormals[(3*indices[i+1])+1];
        face.normals[1].points[2] = vertexnormals[(3*indices[i+1])+2];

        face.normals[2].points[0] = vertexnormals[(3*indices[i+2])];
        face.normals[2].points[1] = vertexnormals[(3*indices[i+2])+1];
        face.normals[2].points[2] = vertexnormals[(3*indices[i+2])+2];

        face.material = facematerials[i/3];
        faces[idx] = face;
        idx++;
    }
}
