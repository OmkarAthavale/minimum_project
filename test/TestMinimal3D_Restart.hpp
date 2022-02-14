#ifndef TESTMINIMAL3DRESTART_HPP_
#define TESTMINIMAL3DRESTART_HPP_

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

//#include "../src/BidomainProblemNeural.hpp"
#include "MonodomainProblem.hpp"
#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"

#include "../src/CardiacSimulationArchiverNeural.hpp"


#include "PetscSetupAndFinalize.hpp"

class TestMinimal3DRestart : public CxxTest::TestSuite
{
  public:
  
  void TestRestarting() throw(Exception)
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "stom_bath.1";
    std::string chkpt_dir = mesh_ident + "-thickenedICC";
    double added_duration = 30000.0;      // ms
    double print_step = 100.0;        // ms
    std::string output_dir = chkpt_dir + "_add30s";
    // ---------------------------------------- //

    MonodomainProblem<PROBLEM_SPACE_DIM>* p_monodomain_problem = CardiacSimulationArchiverNeural< MonodomainProblem<PROBLEM_SPACE_DIM> >::Load(chkpt_dir + "/checkpoint_problem");

    // Heart config changes
   
    HeartConfig::Instance()->SetSimulationDuration(p_monodomain_problem->GetCurrentTime() + added_duration); //ms
    HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(0.1, 0.1, print_step);
    HeartConfig::Instance()->SetOutputDirectory(output_dir);

    p_monodomain_problem->Solve();

    CardiacSimulationArchiverNeural< MonodomainProblem<PROBLEM_SPACE_DIM> >::Save(*p_monodomain_problem, output_dir + "/checkpoint_problem");

    delete p_monodomain_problem;

  };
  
};



#endif /*TESTMINIMAL3DRESTART_HPP_*/