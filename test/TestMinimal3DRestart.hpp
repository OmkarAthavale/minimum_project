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
#include "../src/ICCFactory3D.hpp"

#include "../src/BidomainProblemNeural.hpp"

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
    std::string mesh_ident = "stom_bath_cm";
    std::string chkpt_dir = mesh_ident + "-3DChkpt";
    double added_duration = 50.0;      // ms
    double print_step = 10.0;        // ms
    std::string output_dir = chkpt_dir + "_res" + "_test";
    // ---------------------------------------- //

    BidomainProblemNeural<PROBLEM_SPACE_DIM>* p_bidomain_problem = CardiacSimulationArchiverNeural< BidomainProblemNeural<PROBLEM_SPACE_DIM> >::Load(chkpt_dir + "/checkpoint_problem");

    HeartConfig::Instance()->SetSimulationDuration(p_bidomain_problem->GetCurrentTime() + added_duration); //ms
    HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(0.1, 0.1, print_step);
    HeartConfig::Instance()->SetOutputDirectory(output_dir);

    p_bidomain_problem->Solve();

    delete p_bidomain_problem;

  };
  
};



#endif /*TESTMINIMAL3DRESTART_HPP_*/