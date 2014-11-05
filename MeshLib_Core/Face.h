#ifndef _FACE_H_
#define _FACE_H_

#include <string>
#include "Point.h"
class Halfedge;

class Face
{
public:
	Face(){ m_halfedge = NULL; m_propertyIndex=-1; m_islocked = false;}
	~Face(){;}

	//Pointers for Halfedge Data Structure
	Halfedge    *	& he() { return m_halfedge; }
    Halfedge    *	he() const { return m_halfedge; }
	//optional
	int				& index() {return m_propertyIndex; }
	std::string		& PropertyStr() { return m_propertyStr;}
    
	void setColor(const Point& color) {
		if (!m_islocked) {
			m_color = color;
		}
	}
    const Point& getColor() const { return m_color;}
	void lock() { m_islocked = true; }
	void unlock() { m_islocked = false; }

protected:
	//for Halfedge Data Structure
	Halfedge	*	m_halfedge;

	//optional	
	std::string		m_propertyStr;
	int				m_propertyIndex; // index to Property array
    Point             m_color;
    bool			m_islocked;
};


#endif