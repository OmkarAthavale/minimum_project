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

#include "ChasteEllipsoid.hpp"
#include "ChastePoint.hpp"

#include "../src/DummyDerivedCa.hpp"
#include "../src/Du2013_neural.hpp"

#include "AbstractCardiacCellFactory.hpp"
#include "../src/BidomainProblemNeural.hpp"

#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"

#include "../src/CardiacSimulationArchiverNeural.hpp"


#include "PetscSetupAndFinalize.hpp"

class ICCFactory : public AbstractCardiacCellFactory<PROBLEM_SPACE_DIM>
{
  private:
  std::set<unsigned> setICCNode;

  public:
  ICCFactory(std::set<unsigned> iccNodes) : AbstractCardiacCellFactory<PROBLEM_SPACE_DIM>(), setICCNode(iccNodes)
  {
  };

  AbstractCardiacCell* CreateCardiacCellForTissueNode(Node<PROBLEM_SPACE_DIM>* pNode)
  {
    unsigned index = pNode->GetIndex();

    ChastePoint<PROBLEM_SPACE_DIM> centre(-0.6,-1.1,-3.1);
    ChastePoint<PROBLEM_SPACE_DIM> radii (0.3,0.3, 0.3);
    ChasteEllipsoid<PROBLEM_SPACE_DIM> pacemaker(centre, radii);
    
    if(setICCNode.find(index) != setICCNode.end())
    {
      CellDu2013_neuralFromCellML* cell = new CellDu2013_neuralFromCellML(mpSolver, mpZeroStimulus);
      
      if (pacemaker.DoesContain(pNode->GetPoint()))
      {
        cell->SetParameter("correction", 1.4);
      }

      return cell;

    }

    return new DummyDerivedCa(mpSolver, mpZeroStimulus);

  };
};

class TestMinimal3DRestart : public CxxTest::TestSuite
{
  public:
  
  void TestRestarting()
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "rat_ventCorpus";
    std::string output_dir = mesh_ident + "-3DChkpt";
    double added_duration = 30000.0;      // ms
    double print_step = 100.0;              //ms
    // ---------------------------------------- //

    BidomainProblemNeural<PROBLEM_SPACE_DIM>* p_bidomain_problem = CardiacSimulationArchiverNeural< BidomainProblemNeural<PROBLEM_SPACE_DIM> >::Load(output_dir + "/checkpoint_problem");

    HeartConfig::Instance()->SetSimulationDuration(p_bidomain_problem->GetCurrentTime() + added_duration); //ms
    HeartConfig::Instance()->SetOdePdeAndPrintingTimeSteps(0.1, 0.1, print_step);
    HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(0.000005, 0.005, 0.075)); // these are quite smaller than cm values
    HeartConfig::Instance()->SetExtracellularConductivities(Create_c_vector(0.000005, 0.005, 0.075)); // these are quite smaller than cm values

    p_bidomain_problem->Solve();

    delete p_bidomain_problem;

  };
  
};



#endif /*TESTMINIMAL3DRESTART_HPP_*/