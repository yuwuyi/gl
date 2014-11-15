#include "MeshData.h"

#include "MeshLib_Core/Face.h"
#include "MeshLib_Core/Iterators.h"

#include "Log/log.h"
#include "kdtree/kdtree.h"

#include "Log/log.h"

static void barycentric(Face *f, const Point& point, float  (&lambda)[3]) {
	Point points [3] ;
	int i = 0;
	for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit, ++i) {
		points[i] = (*fvit)->point();
	}

	double x = point[0], y = point[1];
	double x1 = points[0][0], y1 = points[0][1];
	double x2 = points[1][0], y2 = points[1][1];
	double x3 = points[2][0], y3 = points[2][1];

	double dimo = ((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
	lambda[0] = ((y2-y3)*(x-x3) + (x3-x2)*(y-y3)) / dimo;
	lambda[1] = ((y3-y1)*(x-x3) + (x1-x3)*(y-y3)) / dimo;
	lambda[2] = 1.0 - lambda[0] - lambda[1];
}

MeshData::MeshData(Mesh *mesh) : m_mesh(mesh), ptree(nullptr), presults(nullptr) {

}

MeshData::~MeshData() {
	if (presults) {
		kd_res_free(presults );
        presults = 0;
	}

	kd_free( ptree );
	glDeleteBuffers(1, &vboVerticesID);
	glDeleteBuffers(1, &vboColorID);
	
	glDeleteVertexArrays(1, &vaoID);

	delete m_mesh;
}

void MeshData::calculateCenter() {
    glm::vec2 minp(1e6, 1e6), maxp(-1e6, -1e6);
    
    for (MeshVertexIterator mvit(m_mesh); !mvit.end(); ++mvit) {
        Vertex *v = *mvit;
        glm::vec2 p = glm::vec2(float(v->point()[0]), float(v->point()[1]));
        
        if (p.x < minp.x) { minp.x = p.x;}
        if (p.y < minp.y) { minp.y = p.y;}
        if (p.x > maxp.x) { maxp.x = p.x;}
        if (p.y > maxp.y) { maxp.y = p.y;}
    }
    
    center = (minp + maxp) / 2.0f;
    diameter = 2 * std::max(glm::length(minp-center), glm::length(maxp-center));

}


Face *MeshData::locate(Vertex *vertex, const Point& point) {
	Face *inFace = nullptr;
	for (VertexFaceIterator vfit(vertex); !vfit.end(); ++vfit) {
		inFace = *vfit;
		float lambda[3];
		barycentric(inFace, point, lambda);
		for (int i = 0; i < 3; ++i) {
			if (lambda[i] < 0 || lambda[i] > 1) {
				inFace = nullptr;
				break;
			}
		}
		if (inFace) {
			break;
		}
	}

	return inFace;
}


Face *MeshData::locate(double pos[3]) {
    presults = kd_nearest(ptree, pos);
    
    if (!presults) {
        return nullptr;
    }
    
	Point pt(pos[0], pos[1], pos[2]);
    Vertex *v = (Vertex*)kd_res_item( presults, pos );
	
    return locate(v, pt);

}

void MeshData::buildKDTree() {
	ptree = kd_create( 3 );

	/* add some random nodes to the tree (assert nodes are successfully inserted) */
	for(MeshVertexIterator mvit(m_mesh); !mvit.end(); ++mvit) {
		Vertex *v = *mvit;
		if ( 0 != kd_insert3( ptree, v->point()[0], v->point()[1], v->point()[2], v ) ) {
			logutil::fatal("construct kd tree fail!");
			exit(-1);
		}
	}
}