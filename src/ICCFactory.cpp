#include "ICCFactory.hpp"

template<unsigned DIM>
AbstractCardiacCell* ICCFactory<DIM>::CreateCardiacCellForTissueNode(Node<DIM>* pNode)
{
  unsigned index = pNode->GetIndex();


  ChasteEllipsoid<DIM> pacemaker(*pm_centre, *pm_radius);
  
  if(setICCNode.find(index) != setICCNode.end())
  {
    CellDu2013_CalibNeurFromCellML* cell = new CellDu2013_CalibNeurFromCellML(this->mpSolver, this->mpZeroStimulus);
    
    // double xLoc = pNode->GetPoint()[0];
    // double zLoc = pNode->GetPoint()[2];

    // double xRef = 0.0;
    // double xExt = 0.9;

    // double zRef = -3.1;
    // double zExt = -4.7;

    // double pkVal = 1.0;
    // double minVal = 0.9;

    // double gradVal = pkVal - (pkVal-minVal) * (fabs(xLoc-xRef)/(xExt-xRef) )*  (fabs(zLoc-zRef)/(zExt-zRef));

    // if (zLoc < -4.7) {
    //   gradVal = 0.93;
    // }

    // cell->SetParameter("correction", gradVal);
    cell->SetParameter("eta", 0.037);
    if (pacemaker.DoesContain(pNode->GetPoint()))
    {
      cell->SetParameter("correction", 1.075);
      
    }

    return cell;

  }

  return new DummyDerivedCa(this->mpSolver, this->mpZeroStimulus);

}

// Explicit instantiation
template class ICCFactory<1>;
template class ICCFactory<2>;
template class ICCFactory<3>;