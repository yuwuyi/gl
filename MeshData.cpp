#include "MeshData.h"

#include "MeshLib_Core/Face.h"
#include "MeshLib_Core/Iterators.h"

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

MeshData::MeshData(Mesh *mesh) : m_mesh(mesh), ptree(nullptr) {

}

MeshData::~MeshData() {
	if (presults) {
		kd_res_free(presults );
	}

	kd_free( ptree );

	delete m_mesh;
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