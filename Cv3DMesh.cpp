#include "Cv3DMesh.h"

Cv3DMesh::Cv3DMesh(int nVertices,int nFaces)
{
    this->vertices  = new CvMeshVertex[nVertices];
    this->faces     = new CvMeshFace[nFaces];
}
