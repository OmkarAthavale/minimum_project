#ifndef TESTMONODOMAIN3DNEURAL_HPP_
#define TESTMONODOMAIN3DNEURAL_HPP_

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
#include "../src/ICCFactoryFull.hpp"

#include "MonodomainProblemNeural.hpp"

#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"

#include "../src/CardiacSimulationArchiverNeural.hpp"


#include "PetscSetupAndFinalize.hpp"

class TestMonodomain3D : public CxxTest::TestSuite
{
  public:
  void TestMinimalSimulation() throw (Exception)
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "rat_scaffold_16_16_2.1";
    std::string output_dir = mesh_ident + "-3dFull_ICCSMC";
    unsigned bath_attr = 0; // no bath for monodomain
    unsigned icc_attr = 1; // 2=LM, 3=ICC, 4=CM
    double duration = 180000.0;      // ms
    double print_step = 20000.0;        // ms
    // ---------------------------------------- //

    // Mesh location
    std::string mesh_dir = "projects/mesh/scaffold_full/" + mesh_ident;
    TrianglesMeshReader<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM> mesh_reader(mesh_dir.c_str());

    // Initialise mesh variables
    std::set<unsigned> iccNodes;
    unsigned nElements = 0;
    DistributedTetrahedralMesh<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM> mesh;

    // Cell labels
    std::set<unsigned> tissue_id;
    tissue_id.insert(icc_attr);
    // tissue_id.insert(2);
    // tissue_id.insert(3);
    // tissue_id.insert(4);
    std::set<unsigned> bath_id;
    bath_id.insert(bath_attr);

    // Construct ICC mesh network from mesh file
    mesh.ConstructFromMeshReader(mesh_reader);
    
    nElements = mesh.GetNumLocalElements();

    // No need to define boundary nodes as bath for monodomain - but need to specify ICC layers for cell generation
    double eleIdentify = 0;
    for (DistributedTetrahedralMesh<PROBLEM_ELEMENT_DIM,PROBLEM_SPACE_DIM>::ElementIterator iter = mesh.GetElementIteratorBegin(); iter != mesh.GetElementIteratorEnd(); ++iter)
    {
      eleIdentify = iter->GetAttribute();
      if (eleIdentify == icc_attr)
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
    // ParamConfig<PROBLEM_SPACE_DIM>::InitInstance("projects/NeuralData/test3D.txt");
    // ParamConfig<PROBLEM_SPACE_DIM>::GetInstance()->CreateGriddedControlRegions(-1, 1, 4, 0.75, 1.5, 2, -3, -1, 1);
    // ParamConfig<PROBLEM_SPACE_DIM>::GetInstance()->MapNodeToControl(&mesh);


    // Set pacemaker location
    ChastePoint<PROBLEM_SPACE_DIM> centre(0.0, -0.9, -3.0);
    ChastePoint<PROBLEM_SPACE_DIM> radii (0.5, 1.0, 0.3);

    // Initialise problem with cells
    ICCFactoryFull<PROBLEM_SPACE_DIM> network_cells(iccNodes, &centre, &radii);
    MonodomainProblemNeural<PROBLEM_SPACE_DIM> monodomain_problem(&network_cells);
    monodomain_problem.SetMesh( &mesh );

    // Modify simulation config
    HeartConfig::Instance()->Reset();
    HeartConfig::Instance()->SetMeshFileName(mesh_dir, cp::media_type::Orthotropic);
    HeartConfig::Instance()->SetSimulationDuration(duration);
    HeartConfig::Instance()->SetOutputDirectory(output_dir.c_str());
    HeartConfig::Instance()->SetOutputFilenamePrefix("results");
	HeartConfig::Instance()->SetVisualizeWithVtk(true);
    // HeartConfig::Instance()->SetTissueAndBathIdentifiers(tissue_id, bath_id);
    HeartConfig::Instance()->SetUseAbsoluteTolerance(2e-3);
    HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(0.01, 0.3,0.03));
    HeartConfig::Instance()->SetUseAbsoluteTolerance(2e-3); //Changed to get around the DIVERGED_ITS error default:2e-4
    // HeartConfig::Instance()->SetBathConductivity(0.02); // Bath capacitance
    HeartConfig::Instance()->SetSurfaceAreaToVolumeRatio(2000);
    HeartConfig::Instance()->SetCapacitance(2.5);
    HeartConfig::Instance()->SetVisualizeWithMeshalyzer(true);        
    HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(0.1, 0.2, print_step); //timesteps: ode, pde, printing

    // Update problem from config
    monodomain_problem.SetWriteInfo();
    monodomain_problem.Initialise();    // resets initial conditions and time to 0.0 ms

    TRACE("Starting Solve");
    // Solve problem
    monodomain_problem.Solve();

    CardiacSimulationArchiverNeural< MonodomainProblemNeural<PROBLEM_SPACE_DIM> >::Save(monodomain_problem, output_dir + "/checkpoint_problem");

    // Print summary to terminal
    HeartEventHandler::Headings();
    HeartEventHandler::Report();
  };
};

#endif /*TESTMINIMAL_HPP_*/
