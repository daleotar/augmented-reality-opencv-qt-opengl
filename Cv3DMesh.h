#ifndef CV3DMESH_H
#define CV3DMESH_H

typedef struct CvMeshVertex{
    float x;
    float y;
    float z;
}
CvMeshVertex;

typedef struct CvMeshNormal{
    float xDir;
    float yDir;
    float zDir;
}
CvMeshNormal;

typedef struct CvMeshFace{
    CvMeshVertex vertices[3];
    CvMeshNormal normal;
}
CvMeshFace;

class Cv3DMesh
{
public:
    CvMeshVertex *vertices;
    CvMeshFace   *faces;
    Cv3DMesh(int nVertices,int nFaces);
//    loadMesh(QString filename, )
//    computeFaceNormals(CvMeshFace faces[]);
};

#endif // CV3DMESH_H
