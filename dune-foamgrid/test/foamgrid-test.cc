#include <config.h>

#include <dune/grid/test/gridcheck.cc>
#include <dune/grid/test/checkintersectionit.cc>


#include <dune-foamgrid/foamgrid.hh>



using namespace Dune;

template <class GridType>
GridType* make2DHybridTestGrid()
{
    dune_static_assert(GridType::dimension==2,
                       "Instantiate make2dHybridTestGrid only for 2d grids!");

    // Start grid creation
    GridFactory<GridType> factory;
        
    // The list of grid vertex positions
    int numVertices = 16;
    
    double vertices[16][3] = {{0,    0,    0},
                              {0.5,  0,    0},
                              {0.5,  0.5,  0},
                              {0,    0.5,  0},
                              {0.25, 0,    0},
                              {0.5,  0.25, 0},
                              {0.25, 0.5,  0},
                              {0,    0.25, 0},
                              {0.25, 0.25, 0},
                              {1,    0,    0},
                              {1,    0.5,  0},
                              {0.75, 0.25, 0},
                              {1,    1,    0},
                              {0.5,  1,    0},
                              {0,    1,    0},
                              {0.25, 0.75, 0}};
    
    // Create the grid vertices
    for (int i=0; i<numVertices; i++) {
        Dune::FieldVector<double,3> pos;
        pos[0] = vertices[i][0];
        pos[1] = vertices[i][1];
        pos[2] = vertices[i][2];
        factory.insertVertex(pos);
    }
    
    // Create the triangle elements
    int numTriangles = 2;
    unsigned int triangles[2][3] = {{9, 10, 11},
                                    {15, 13, 14}};
    
    for (int i=0; i<numTriangles; i++) {
        std::vector<unsigned int> cornerIDs(3);
        for (int j=0; j<3; j++)
            cornerIDs[j] = triangles[i][j];
        factory.insertElement(Dune::GeometryType(Dune::GeometryType::simplex,2),cornerIDs);
    }
    
#if 0
    // Create the quadrilateral elements
    int numQuadrilaterals = 9;
    unsigned int quadrilaterals[9][4] = {{0, 4, 7, 8},
                                         {4, 1, 8, 5},
                                         {8, 5, 6, 2},
                                         {7, 8, 3, 6},
                                         {1, 9, 5, 11},
                                         {5, 11, 2, 10},
                                         {2, 10, 13, 12},
                                         {3, 6, 14, 15},
                                         {6, 2, 15, 13}};
    
    for (int i=0; i<numQuadrilaterals; i++) {
        std::vector<unsigned int> cornerIDs(4);
        for (int j=0; j<4; j++)
            cornerIDs[j] = quadrilaterals[i][j];
        factory.insertElement(Dune::GeometryType(Dune::GeometryType::cube,2),cornerIDs);
    }
#endif
    
    // Finish initialization
    return factory.createGrid();
}


int main (int argc, char *argv[]) try
{
    typedef FoamGrid GridType;
    GridType* grid = make2DHybridTestGrid<GridType>();

    gridcheck(*grid);
    checkIntersectionIterator(*grid);

} 
// //////////////////////////////////
//   Error handler
// /////////////////////////////////
 catch (Exception e) {

    std::cout << e << std::endl;
    return 1;
 }
