# PCMesh 3D

### Flow of control:

**1. Configure octree**
    
Set optimum cell size

**2. Construct linear octree**
  
  * Leaf node resolution is dictated by optimum cell size
  * Stored as ordered map of Morton encoding &rArr; Cell containing list of points
  * Construction time: O(NlogN)

**3. Sample seed for delaunay triangulation**
	
Sparse uniformly spaced sampling over point set

**4. Construct dual of triangulation**
	
Voronoi cells

**5. Refine triangulation**
	
Check topological consistency and fix with intersections of voronoi edges

**6. Add triangles**
	
Subdivide triangles if boundary triangle edges have sharp angles


### Intersection check algorithm

1. Traverse octree depth-first
2. Qualify octant if voronoi edge intersects octant
3. At leaf nodes, conduct marching cubes to generate isosurface
4. Return intersection point