#ifndef MESH_H
#define MESH_H

struct Vertex{
    float points[3];
};

struct Face{
    int numvertex;
    Vertex *vertex;
    Vertex *normals;
    int material;
};

class Mesh
{
public:
    Mesh(unsigned nbvertex=0,unsigned nbfaces=0,unsigned nbtexcords=0,unsigned nbnormals=0, unsigned nbmaterials=0);
    Mesh(const Mesh &copy);
    ~Mesh();
    void generateFaceNormals();
    void generateVertexNormals();
    unsigned numVertex(){return nbVertex;}
    unsigned numIndices(){return nbFaces;}
    bool hasTextCords(){return nbTexCords;}
    bool hasNormals(){return nbNormals;}
    void separateFaces();

public:
    //info
    unsigned nbVertex, nbFaces, nbTexCords, nbNormals, nbMaterials;
    //data
    float *vertices,*textcords,*facenormals,*vertexnormals,*facecenters,*materials,*matalpha;
    Face *faces;
    unsigned int *indices, *facematerials;
};

#endif // MESH_H
