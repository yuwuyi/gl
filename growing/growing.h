#ifndef growing_h__
#define growing_h__

#include "../MeshLib_Core/Iterators.h"
#include "../MeshLib_Core/Halfedge.h"
#include "../MeshLib_Core/Point.h"

#include <set>
#include <map>
#include <vector>
#include <algorithm>

class Mesh;

inline Point calcCenter(const Face *face) {
	Point point;
	for (FaceVertexIterator fvit(face); !fvit.end(); ++fvit) {
		Vertex *v = *fvit;
		point += v->point();
	}

	point /= 3.0;
	return point;
}

inline double calcLinfinity(const Point & p1, const Point& p2) {
	Point p = p1 - p2;
    p[0] = fabs(p[0]);
    p[1] = fabs(p[1]);
    p[2] = fabs(p[2]);
	return *std::max_element(p.v, p.v + 2);
}

inline double calcL2(const Point & p1, const Point& p2) {
    Point p = p1 - p2;
    return p.norm();
}

struct GrowingPatch {
	Point center, color;
	std::vector<Halfedge*> frontEdges;

	bool operator()(const Face*f0, const Face* f1) const {
		Point c0 = calcCenter(f0);
		Point c1 = calcCenter(f1);
		double d0 = calcLinfinity(c0, center);
		double d1 = calcLinfinity(c1, center);
		return d0 < d1;
	}
};

class RegionGrowing {
public:
	RegionGrowing() {}
	~RegionGrowing() {
		for (size_t i = 0; i <m_patches.size(); ++i) {
			delete m_patches[i];
		}
	}
	void initPatches(Face *face);
    void initPatches(Mesh *mesh, int N);
	bool grow();
	
private:
	bool grow(GrowingPatch *patch);

	std::vector<GrowingPatch*> m_patches;
	std::map<Face*, GrowingPatch*> face2patchMap;
};

#endif // growing_h__
