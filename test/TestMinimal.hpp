#ifndef TESTMINIMAL_HPP_
#define TESTMINIMAL_HPP_

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

#include "ChasteEllipsoid.hpp"
#include "ChastePoint.hpp"

#include "../src/DummyDerivedCa.hpp"
#include "../src/Du2013_neural.hpp"

#include "AbstractCardiacCellFactory.hpp"
#include "../src/BidomainProblemNeural.hpp"

#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"

#include "../src/CardiacSimulationArchiverNeural.hpp"
#include "../src/ParamConfig.hpp"

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

    ChastePoint<PROBLEM_SPACE_DIM> centre(0.02,0.04);
    ChastePoint<PROBLEM_SPACE_DIM> radii (0.01,0.03);
    ChasteEllipsoid<PROBLEM_SPACE_DIM> pacemaker(centre, radii);
    
    if(setICCNode.find(index) != setICCNode.end())
    {
      CellDu2013_neuralFromCellML* cell = new CellDu2013_neuralFromCellML(mpSolver, mpZeroStimulus);
      
      if (pacemaker.DoesContain(pNode->GetPoint()))
      {
        cell->SetParameter("correction", 1.05);
      }

      return cell;

    }

    return new DummyDerivedCa(mpSolver, mpZeroStimulus);

  };
};

class TestMinimal : public CxxTest::TestSuite
{
  public:
  void TestMinimalSimulation()
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "MeshNetwork-2D-85Nodes-144Elems";
    std::string output_dir = mesh_ident + "-2DParamConfig";
    unsigned bath_attr = 0;
    unsigned icc_attr = 1;
    double duration = 25000.0;      // ms
    double print_step = 100.0;        // ms
    // ---------------------------------------- //

    // Mesh location
    std::string mesh_dir = "projects/mesh/ICC2D/" + mesh_ident;
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
    ParamConfig::InitInstance("projects/NeuralData/testData.txt");
    // ParamConfig::CreateGriddedControlRegions(0.0, 2.0, 20, 0.0, 3.0, 30);
    ParamConfig::GetInstance()->CreateGriddedControlRegions(0.0, 0.055, 3, 0.0, 0.055, 3);
    ParamConfig::GetInstance()->MapNodeToControl(mesh);

    // Initialise problem with cells
    ICCFactory network_cells(iccNodes);
    BidomainProblemNeural<PROBLEM_SPACE_DIM> bidomain_problem(&network_cells, true);
    bidomain_problem.SetMesh( &mesh );

    // Modify simulation config
    HeartConfig::Instance()->Reset();
    HeartConfig::Instance()->SetSimulationDuration(duration);
    HeartConfig::Instance()->SetOutputDirectory(output_dir.c_str());
    HeartConfig::Instance()->SetOutputFilenamePrefix("results");
    HeartConfig::Instance()->SetTissueAndBathIdentifiers(ICC_id, bath_id);
    HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(0.12, 0.12)); // these are quite smaller than cm values
    HeartConfig::Instance()->SetExtracellularConductivities(Create_c_vector(0.2, 0.2)); // these are quite smaller than cm values
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

    CardiacSimulationArchiverNeural< BidomainProblemNeural<PROBLEM_SPACE_DIM> >::Save(bidomain_problem, output_dir + "/checkpoint_problem");

    // Print summary to terminal
    HeartEventHandler::Headings();
    HeartEventHandler::Report();
  };

  void xTestRestarting()
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "MeshNetwork-2D-85Nodes-144Elems";
    std::string output_dir = mesh_ident + "-2DChkpt";
    double added_duration = 10000.0;      // ms
    // ---------------------------------------- //

    BidomainProblemNeural<PROBLEM_SPACE_DIM>* p_bidomain_problem = CardiacSimulationArchiverNeural< BidomainProblemNeural<PROBLEM_SPACE_DIM> >::Load(output_dir + "checkpoint_problem");

    HeartConfig::Instance()->SetSimulationDuration(p_bidomain_problem->GetCurrentTime() + added_duration); //ms

    p_bidomain_problem->Solve();

    delete p_bidomain_problem;

  };

};

#endif /*TESTMINIMAL_HPP_*/
