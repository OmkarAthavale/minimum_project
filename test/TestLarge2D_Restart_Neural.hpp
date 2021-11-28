#ifndef TESTLARGE2D_RESTART_NEURAL_HPP_
#define TESTLARGE2D_RESTART_NEURAL_HPP_

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

#include "ChastePoint.hpp"
#include "../src/ICCFactory_Large2D_Neural.hpp"

#include "../src/BidomainProblemNeural.hpp"

#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"

#include "../src/CardiacSimulationArchiverNeural.hpp"


#include "PetscSetupAndFinalize.hpp"

class TestLarge2D_Restart_Neural : public CxxTest::TestSuite
{
  public:
  void TestRestarting() throw(Exception)
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "MeshNetwork-2D-2147Nodes-4160Elems";
    std::string chkpt_dir = mesh_ident + "-2D_Large_Base";
    double added_duration = 120000.0;      // ms
    double print_step = 100.0;        // ms
    std::string output_dir = chkpt_dir + "_neural120s";
    // ---------------------------------------- //

    BidomainProblemNeural<PROBLEM_SPACE_DIM>* p_bidomain_problem = CardiacSimulationArchiverNeural< BidomainProblemNeural<PROBLEM_SPACE_DIM> >::Load(chkpt_dir + "/checkpoint_problem");

    // Loads neural info and set up ParamConfig singleton instance
    ParamConfig::InitInstance("projects/NeuralData/large2D_test.txt");
    ParamConfig::GetInstance()->CreateGriddedControlRegions(0.0, 2.0, 20, 0.0, 3.0, 30);
    ParamConfig::GetInstance()->MapNodeToControl(&(p_bidomain_problem->rGetMesh()));

    // Heart config changes
    HeartConfig::Instance()->SetSimulationDuration(p_bidomain_problem->GetCurrentTime() + added_duration); //ms
    HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(0.1, 0.1, print_step);
    HeartConfig::Instance()->SetOutputDirectory(output_dir);

    p_bidomain_problem->Solve();

    delete p_bidomain_problem;

  };
  
};



#endif /*TESTMINIMAL3DRESTART_HPP_*/