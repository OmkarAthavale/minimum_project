#ifndef TESTPARAMCONFIG_HPP_
#define TESTPARAMCONFIG_HPP_

#define PROBLEM_SPACE_DIM 3
#define PROBLEM_ELEMENT_DIM 3
/**
 * @file
 * This test runs a minimal simulation with Simplified Imtiaz cells in a 2D mesh
 */

#include <cxxtest/TestSuite.h>
/* Most Chaste code uses PETSc to solve linear algebra problems.  This involves starting PETSc at the beginning of a test-suite
 * and closing it at the end.  (If you never run code in parallel then it is safe to replace PetscSetupAndFinalize.hpp with FakePetscSetup.hpp)
 */

#include "Debug.hpp"

#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"
#include "../src/ParamConfig.hpp"
#include "PetscSetupAndFinalize.hpp"

class TestMinimal : public CxxTest::TestSuite
{
  public:
  void TestMinimalSimulation()
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "rat_scaffold_section_16_16_2.1";
    std::string output_dir = mesh_ident + "-testingCondEta";
    // ---------------------------------------- //

    // Mesh location
    std::string mesh_dir = "projects/mesh/scaffold/" + mesh_ident;
    TrianglesMeshReader<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM> mesh_reader(mesh_dir.c_str());

    // Initialise mesh variables
    DistributedTetrahedralMesh<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM> mesh;

    // Construct ICC mesh network from mesh file
    mesh.ConstructFromMeshReader(mesh_reader);

    // Load parameter confg text file
    ParamConfig<PROBLEM_SPACE_DIM>::InitInstance("projects/NeuralData/test3D.txt");
    ParamConfig<PROBLEM_SPACE_DIM>::GetInstance()->CreateGriddedControlRegions(-1, 1, 20, -1.5, 0.75, 30, -4.8, -3, 1);
    ParamConfig<PROBLEM_SPACE_DIM>::GetInstance()->MapNodeToControl(&(mesh));
  };

};

#endif
