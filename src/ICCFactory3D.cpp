#include "ICCFactory3D.hpp"

AbstractCardiacCell* ICCFactory3D::CreateCardiacCellForTissueNode(Node<PROBLEM_SPACE_DIM>* pNode)
{
  unsigned index = pNode->GetIndex();


  ChasteEllipsoid<PROBLEM_SPACE_DIM> pacemaker(*pm_centre, *pm_radius);
  
  if(setICCNode.find(index) != setICCNode.end())
  {
    CellDu2013_neuralFromCellML* cell = new CellDu2013_neuralFromCellML(mpSolver, mpZeroStimulus);
    
    if (pacemaker.DoesContain(pNode->GetPoint()))
    {
      cell->SetParameter("correction", 1.15);
    }

    return cell;

  }

  return new DummyDerivedCa(mpSolver, mpZeroStimulus);

}