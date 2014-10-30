#include "growing.h"
#include <iostream>
#include <time.h>


#include "../MeshLib_Core/Mesh.h"

void RegionGrowing::initPatches(Face *face) {
	GrowingPatch *p = new GrowingPatch;

    face2patchMap[face] = p;
	p->center = calcCenter(face);
	for (FaceHalfedgeIterator fheit(face); !fheit.end(); ++fheit) {
		p->frontEdges.push_back(*fheit);
	}
    m_patches.push_back(p);
}

bool RegionGrowing::grow() {
	for (size_t i = 0; i < m_patches.size(); ++i) {
		GrowingPatch *patch = m_patches[i];
        grow(patch);
	}
	return true;
}


void RegionGrowing::initPatches(Mesh *mesh, int N) {
    srand (static_cast <unsigned> (time(0)));

    for (int i = 0; i < N; ++i) {
        GrowingPatch *p = new GrowingPatch;

        float r = static_cast <float> (rand() % 100) / static_cast <float> (100);
        float g = static_cast <float> (rand()% 100) / static_cast <float> (100);
        float b = static_cast <float> (rand()% 100) / static_cast <float> (100);

        p->color = Point(r, g, b);



        int fid = rand() % mesh->numFaces() + 1;
        Face *face = mesh->indFace(fid);
        face2patchMap[face] = p;
        p->center = calcCenter(face);
        for (FaceHalfedgeIterator fheit(face); !fheit.end(); ++fheit) {
            p->frontEdges.push_back(*fheit);
        }

        m_patches.push_back(p);

    }




}

bool RegionGrowing::grow(GrowingPatch *patch) {

	std::set<Face*, GrowingPatch> candidateFaces(*patch);
	std::set<Halfedge*> candidateFrontHalfEdges;
	for (size_t i = 0; i < patch->frontEdges.size(); ++i) {
		Halfedge *he = patch->frontEdges[i];
		Face *f0 = he->face();
        Point cf0 = calcCenter(f0);
        std::cout << "cf0 " <<cf0[0] << ", " << cf0[1] << "\n";
        std::cout << "pc0 " << patch->center[0] << ", " <<patch->center[1] << std::endl;
        double ldist0 = calcLinfinity(calcCenter(f0), patch->center);
        double l2dist0 = calcL2(calcCenter(f0), patch->center);
        if (ldist0 < 20 && !face2patchMap[f0]) {
            candidateFrontHalfEdges.insert(he);
            candidateFaces.insert(f0);
        }


        Halfedge *ohe = he->twin();
		if (!ohe) {
			continue;
		}
		Face *f1 = ohe->face();
        double ldist1 = calcLinfinity(calcCenter(f1), patch->center);
        double l2dist1 = calcL2(calcCenter(f1), patch->center);


		if (ldist1 < 20  && !face2patchMap[f1]) {
			candidateFrontHalfEdges.insert(ohe);
			candidateFaces.insert(f1);
		}

        std::cout << "ldist0 " << ldist0 << " " << "ldist1 " << ldist1 << "\n";
	}

	//update the map
	for (auto f : candidateFaces) {
		face2patchMap[f] = patch;
		f->PropertyStr() =  "rgb=(1 0 0)";
        f->setColor(patch->color);

        for (FaceHalfedgeIterator fheit(f); !fheit.end(); ++fheit) {
            candidateFrontHalfEdges.insert(*fheit);
        }
	}

	patch->frontEdges.clear();
	//update the front edge;
	for (auto he : candidateFrontHalfEdges) {
		Face *f0 = he->face();
		Halfedge *ohe = he->twin();
		if (ohe) {
			Face *f1 = ohe->face();
			if (!face2patchMap[f0]) {
				patch->frontEdges.push_back(he);
			}
			if (!face2patchMap[f1]) {
				patch->frontEdges.push_back(ohe);
			}
		}
	}



	return true;
}