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

#include "ChasteEllipsoid.hpp"
#include "ChastePoint.hpp"

#include "../src/DummyDerivedCa.hpp"
#include "../src/Du2013_neural.hpp"

#include "AbstractCardiacCellFactory.hpp"
#include "../src/BidomainProblemNeural.hpp"

#include "DistributedTetrahedralMesh.hpp"
#include "TrianglesMeshReader.hpp"

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

    ChastePoint<PROBLEM_SPACE_DIM> centre(-6,-11,-31);
    ChastePoint<PROBLEM_SPACE_DIM> radii (3, 3, 3);
    ChasteEllipsoid<PROBLEM_SPACE_DIM> pacemaker(centre, radii);
    
    if(setICCNode.find(index) != setICCNode.end())
    {
      CellDu2013_neuralFromCellML* cell = new CellDu2013_neuralFromCellML(mpSolver, mpZeroStimulus);
      
      TRACE(pNode->GetPoint()[0] << ", " << pNode->GetPoint()[1] << ", " << pNode->GetPoint()[2]);
      if (pacemaker.DoesContain(pNode->GetPoint()))
      {
        cell->SetParameter("correction", 1.4);
      }

      return cell;

    }

    return new DummyDerivedCa(mpSolver, mpZeroStimulus);

  };
};

class TestMinimal3D : public CxxTest::TestSuite
{
  public:
  void TestMinimalSimulation()
  {

    // -------------- OPTIONS ----------------- //
    std::string mesh_ident = "rat_ventCorpus";
    std::string output_dir = mesh_ident + "-3DSerial";
    unsigned bath_attr = 1;
    unsigned icc_attr = 2;
    double duration = 1000.0;      // ms
    double print_step = 50.0;        // ms
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
      if (eleIdentify == icc_attr) // ICC=1 and Bath=2
      {
        for(int j = 0; j<=3; ++j)
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
    HeartConfig::Instance()->SetIntracellularConductivities(Create_c_vector(0.00005, 0.75, 0.05)); // these are quite smaller than cm values
    HeartConfig::Instance()->SetExtracellularConductivities(Create_c_vector(0.00005, 0.75, 0.05)); // these are quite smaller than cm values
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
