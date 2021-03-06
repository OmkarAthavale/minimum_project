#ifndef TESTMINIMAL3D_HPP_
#define TESTMINIMAL3D_HPP_

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

#include "ChastePoint.hpp"
#include "../src/ICCFactory.hpp"

#include "../src/BidomainProblemNeural.hpp"

#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"


#include "PetscSetupAndFinalize.hpp"

class TestMinimal3D : public CxxTest::TestSuite
{
  public:
  void TestMinimalSimulation()
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "stom_bath_cm";
    std::string output_dir = mesh_ident + "-3DcorrectionPM";
    unsigned bath_attr = 1;
    unsigned icc_attr = 2;
    double duration = 90000.0;      // ms
    double print_step = 3000.0;        // ms
    // ---------------------------------------- //

    // Mesh location
    std::string mesh_dir = "projects/mesh/ICC3D/" + mesh_ident;
    TrianglesMeshReader<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM> mesh_reader(mesh_dir.c_str());

    // Initialise mesh variables
    std::set<unsigned> iccNodes;
    unsigned nElements = 0;
    DistributedTetrahedralMesh<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM> mesh;

    // Cell labels
    std::set<unsigned> ICC_id;
    ICC_id.insert(icc_attr);
    std::set<unsigned> bath_id;
    bath_id.insert(bath_attr);

    // Construct ICC mesh network from mesh file
    mesh.ConstructFromMeshReader(mesh_reader);
    
    nElements = mesh.GetNumLocalElements();

    // Define boundary nodes as bath
    double eleIdentify = 0;
    for (DistributedTetrahedralMesh<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM>::ElementIterator iter = mesh.GetElementIteratorBegin(); iter != mesh.GetElementIteratorEnd(); ++iter)
    {
      eleIdentify = iter->GetAttribute();
      if (eleIdentify == icc_attr) // ICC=2 and Bath=1
      {
        for(int j = 0; j<=PROBLEM_ELEMENT_DIM; ++j)
        {
            iccNodes.insert(iter->GetNodeGlobalIndex(j));
        }
      }
    }

    // Print mesh summary
    TRACE("Number of elements: " << nElements);
    TRACE("Number of ICC nodes: " << iccNodes.size());
    TRACE("Total number of nodes: " << mesh.GetNumAllNodes());

    // TODO: Load neural info and set up ParamConfig singleton instance
    // NeuralData eData = NeuralData("projects/NeuralData/T1_e.txt", 1, 2, 3, 0.12, 0.36); // this will be a pre-processed (in Python) table instead of a histogram
    // maybe possible to call Python pre-processor on the fly at runtime (future work)
    // ParamConfig::SetExcitatory("path/to/preprocessed_e.txt");
    // ParamConfig::SetInhibitory("path/to/preprocessed_i.txt");
    // ParamConfig::SetupInfluenceRegionGrid(width, length, width_bins, length_bins)
    // ParamConfig::SetInputTimestep(stepInMillisec)


    // Set pacemaker location
    ChastePoint<PROBLEM_SPACE_DIM> centre(-0.6, -1.1, -3.1);
    ChastePoint<PROBLEM_SPACE_DIM> radii (0.3, 0.3, 0.3);

    // Initialise problem with cells
    ICCFactory<3> network_cells(iccNodes, &centre, &radii);
    BidomainProblemNeural<PROBLEM_SPACE_DIM> bidomain_problem(&network_cells, true);
    bidomain_problem.SetMesh( &mesh );

    // Modify simulation config
    HeartConfig::Instance()->Reset();
    HeartConfig::Instance()->SetSimulationDuration(duration);
    HeartConfig::Instance()->SetOutputDirectory(output_dir.c_str());
    HeartConfig::Instance()->SetOutputFilenamePrefix("results");
    HeartConfig::Instance()->SetTissueAndBathIdentifiers(ICC_id, bath_id);
    HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(0.00005, 0.05, 0.75));
    HeartConfig::Instance()->SetExtracellularConductivities(Create_c_vector(0.00005, 0.05, 0.75));
    HeartConfig::Instance()->SetSurfaceAreaToVolumeRatio(2000);
    HeartConfig::Instance()->SetCapacitance(2.5);
    HeartConfig::Instance()->SetVisualizeWithMeshalyzer(true);
    HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(0.1, 0.1, print_step);

    // Update problem from config
    bidomain_problem.SetWriteInfo();
    bidomain_problem.Initialise();    // resets initial conditions and time to 0.0 ms

    TRACE("Starting Solve");
    // Solve problem
    bidomain_problem.Solve();

    // Print summary to terminal
    HeartEventHandler::Headings();
    HeartEventHandler::Report();
  };
};

#endif /*TESTMINIMAL_HPP_*/
