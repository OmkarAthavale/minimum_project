#include "ICCFactory.hpp"

template<unsigned DIM>
AbstractCardiacCell* ICCFactory<DIM>::CreateCardiacCellForTissueNode(Node<DIM>* pNode)
{
  unsigned index = pNode->GetIndex();


  ChasteEllipsoid<DIM> pacemaker(*pm_centre, *pm_radius);
  
  if(setICCNode.find(index) != setICCNode.end())
  {
    CellDu2013_neuralFromCellML* cell = new CellDu2013_neuralFromCellML(this->mpSolver, this->mpZeroStimulus);

    return cell;

  }

  return new DummyDerivedCa(this->mpSolver, this->mpZeroStimulus);

}

// Explicit instantiation
template class ICCFactory<1>;
template class ICCFactory<2>;
template class ICCFactory<3>;