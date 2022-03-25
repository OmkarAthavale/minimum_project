#ifndef TESTPARAMCONFIG_HPP_
#define TESTPARAMCONFIG_HPP_

#define PROBLEM_SPACE_DIM 2
#define PROBLEM_ELEMENT_DIM 2
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
    std::string mesh_ident = "MeshNetwork-2D-85Nodes-144Elems";
    std::string output_dir = mesh_ident + "-2DChkpt";
    unsigned bath_attr = 0;
    unsigned icc_attr = 1;
    // ---------------------------------------- //

    // Mesh location
    std::string mesh_dir = "projects/mesh/ICC2D/" + mesh_ident;
    TrianglesMeshReader<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM> mesh_reader(mesh_dir.c_str());

    // Initialise mesh variables
    std::set<unsigned> iccNodes;
    DistributedTetrahedralMesh<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM> mesh;

    // Cell labels
    std::set<unsigned> ICC_id;
    ICC_id.insert(icc_attr);
    std::set<unsigned> bath_id;
    bath_id.insert(bath_attr);

    // Construct ICC mesh network from mesh file
    mesh.ConstructFromMeshReader(mesh_reader);

    // Define boundary nodes as bath
    double eleIdentify = 0;
    for (DistributedTetrahedralMesh<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM>::ElementIterator iter = mesh.GetElementIteratorBegin(); iter != mesh.GetElementIteratorEnd(); ++iter)
    {
      eleIdentify = iter->GetAttribute();
      if (eleIdentify == icc_attr) // ICC=1 and Bath=0
      {
        if(!iter->GetNode(0)->IsBoundaryNode())
        {
          iccNodes.insert(iter->GetNodeGlobalIndex(0));
        }
        if(!iter->GetNode(1)->IsBoundaryNode())
        {
          iccNodes.insert(iter->GetNodeGlobalIndex(1));
        }
        // 2D has two nodes per element for line elements?
        if(!iter->GetNode(2)->IsBoundaryNode())
        {
          iccNodes.insert(iter->GetNodeGlobalIndex(2));
        }
      }
    }
    // TODO: Load neural info and set up ParamConfig singleton instance
    // NeuralData eData = NeuralData("projects/NeuralData/T1_e.txt", 1, 2, 3, 0.12, 0.36); // this will be a pre-processed (in Python) table instead of a histogram
    // maybe possible to call Python pre-processor on the fly at runtime (future work)
    // ParamConfig::SetExcitatory("path/to/preprocessed_e.txt");
    // ParamConfig::SetInhibitory("path/to/preprocessed_i.txt");
    // ParamConfig::SetupInfluenceRegionGrid(width, length, width_bins, length_bins)
    // ParamConfig::SetInputTimestep(stepInMillisec)
    ParamConfig<PROBLEM_ELEMENT_DIM>::InitInstance("projects/NeuralData/testData.txt");
    // ParamConfig::CreateGriddedControlRegions(0.0, 2.0, 20, 0.0, 3.0, 30);
    ParamConfig<PROBLEM_ELEMENT_DIM>::GetInstance()->CreateGriddedControlRegions(0.0, 0.055, 3, 0.0, 0.055, 3);
    ParamConfig<PROBLEM_ELEMENT_DIM>::GetInstance()->MapNodeToControl(&mesh);
  };

};

#endif /*TESTMINIMAL_HPP_*/
