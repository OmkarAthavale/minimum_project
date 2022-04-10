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
#include "../src/ICCFactoryFull.hpp"

#include "MonodomainProblemNeural.hpp"

#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"

#include "../src/CardiacSimulationArchiverNeural.hpp"


#include "PetscSetupAndFinalize.hpp"

class TestMonodomain3DRestart : public CxxTest::TestSuite
{
  public:
  
  void TestRestarting() throw(Exception)
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "rat_scaffold_16_16_2.1";
    std::string chkpt_dir = mesh_ident + "-3dFull_toSS";
    double added_duration = 5000.0;      // ms
    double print_step = 1000.0;        // ms
    std::string output_dir = chkpt_dir + "_testStim";
    // ---------------------------------------- //
    
    MonodomainProblemNeural<PROBLEM_SPACE_DIM>* p_monodomain_problem = CardiacSimulationArchiverNeural< MonodomainProblemNeural<PROBLEM_SPACE_DIM> >::Load(chkpt_dir + "/checkpoint_problem");
    AbstractTetrahedralMesh<PROBLEM_SPACE_DIM, PROBLEM_ELEMENT_DIM>* pMesh = &(p_monodomain_problem->rGetMesh());
    for (unsigned node_index = 0; node_index<pMesh->GetNumNodes(); node_index++)
    {
      if (pMesh->GetDistributedVectorFactory()->IsGlobalIndexLocal(node_index)) {
        p_monodomain_problem->GetTissue()->GetCardiacCell(node_index)->SetParameter("excitatory_neural", 5.0);
      }
    }
    
    // // Loads neural info and set up ParamConfig singleton instance
    // ParamConfig<PROBLEM_SPACE_DIM>::InitInstance("projects/NeuralData/testLaplaceSet.txt");
    // ParamConfig<PROBLEM_SPACE_DIM>::GetInstance()->CreateGriddedControlRegions(-1, 1, 2, -1.5, 0.75, 2, -4.8, -3, 1);
    // ParamConfig<PROBLEM_SPACE_DIM>::GetInstance()->MapNodeToControl(&(p_monodomain_problem->rGetMesh()));
    // TRACE("gets to here")
    // ParamConfig<PROBLEM_SPACE_DIM>::GetInstance()->MapNodeToControl(&(p_monodomain_problem->rGetMesh()), "projects/mesh/scaffold_full/rat_scaffold_16_16_2.1_laplace_longi_sw.txt", 10, 97, 10);

    HeartConfig::Instance()->SetSimulationDuration(p_monodomain_problem->GetCurrentTime() + added_duration); //ms
    HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(0.01, 0.3,0.03)); // TO MODIFY
    HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(0.1, 0.2, print_step);
    HeartConfig::Instance()->SetOutputDirectory(output_dir);

    p_monodomain_problem->Solve();

    CardiacSimulationArchiverNeural< MonodomainProblemNeural<PROBLEM_SPACE_DIM> >::Save(*p_monodomain_problem, output_dir + "/checkpoint_problem");

    delete p_monodomain_problem;

  };
  
};



#endif /*TESTMINIMAL3DRESTART_HPP_*/