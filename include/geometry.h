#include <context.h>
#include <debugger.h>

namespace geometry {
	enum Axis {X, Y, Z};

	// Length of vector
	float length(Vector v) {
		return CGAL::sqrt(v.squared_length());
	}
	// Distance between two lines
	float distance(Line l1, Line l2) {
		if(CGAL::parallel(l1, l2)) {
			Vector n = CGAL::cross_product(l1.to_vector(), l2.point(0) - l1.point(0)) / length(l1.to_vector());
			return length(n);
		}
		Vector n = CGAL::cross_product(l1.to_vector(), l2.to_vector());
		return std::abs(n * (l1.point(0) - l2.point(0)) / length(n));
	}
	// Helper to check if x belongs to [a,b]
	bool inline isInRange(float x, float a, float b) {
		if(a <= x && x <= b)
			return true;
		return false;
	}
	// Intersection check for line and face
	bool intersects(Line l, Vector min, Vector max, Axis axis)	 {
		Vector v = l.to_vector();
		Point origin = l.point(0);
		Point p;
		float coeff;
		switch(axis) {
			case X:
				if(v.x() == 0) return false;
				p = origin + (min.x() - origin.x()) * v / v.x(); // Point of intersection
				if( isInRange(p.y(), min.y(), max.y()) &&
					isInRange(p.z(), min.z(), max.z())) return true;
				return false;
			case Y:
				if(v.y() == 0) return false;
				p = origin + (min.y() - origin.y()) * v / v.y(); // Point of intersection
				if( isInRange(p.x(), min.x(), max.x()) &&
					isInRange(p.z(), min.z(), max.z())) return true;
				return false;
			case Z:
				if(v.z() == 0) return false;
				p = origin + (min.z() - origin.z()) * v / v.z(); // Point of intersection
				if( isInRange(p.x(), min.x(), max.x()) &&
					isInRange(p.y(), min.y(), max.y())) return true;
				return false;
		}
		return false;
	}
	// Intersection check for line and cube
	bool intersects(Line l, Vector min, Vector max) {
		if( intersects(l, Vector(min.x(), min.y(), min.z()), Vector(min.x(), max.y(), max.z()), X) ||
			intersects(l, Vector(max.x(), min.y(), min.z()), Vector(max.x(), max.y(), max.z()), X) ||
			intersects(l, Vector(min.x(), min.y(), min.z()), Vector(max.x(), min.y(), max.z()), Y) ||
			intersects(l, Vector(min.x(), max.y(), min.z()), Vector(max.x(), max.y(), max.z()), Y) ||
			intersects(l, Vector(min.x(), min.y(), max.z()), Vector(max.x(), max.y(), max.z()), Z))
			return true;
		return false;
	}
}