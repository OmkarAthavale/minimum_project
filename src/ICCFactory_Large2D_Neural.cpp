#include "ICCFactory_Large2D_Neural.hpp"

template<unsigned DIM>
AbstractCardiacCell* ICCFactory_Large2D_Neural<DIM>::CreateCardiacCellForTissueNode(Node<DIM>* pNode)
{
  unsigned index = pNode->GetIndex();


  ChasteCuboid<DIM> fundus(*v1, *v2);
  
  if(setICCNode.find(index) != setICCNode.end())
  {
    CellDu2013_neuralFromCellML* cell = new CellDu2013_neuralFromCellML(this->mpSolver, this->mpZeroStimulus);
    
    if (fundus.DoesContain(pNode->GetPoint()))
    {
      cell->SetParameter("correction", 0.0);
    }

    bool excitabilityGradient = false;
    if (excitabilityGradient) {
      double x = pNode->GetPoint()[0];
      double y = pNode->GetPoint()[1];

      double p00 = -70.98;
      double p10 = 5.137;
      double p01 = -1.34;
      double p20 = -2.569;
      double p11 = -1.712;
      double p02 = -3.246e-07;
      double p21 = 0.8562;
      double p12 = -7.653e-15;
      double p03 = 5.549e-08;

      cell->SetParameter("E_K",  p00 + p10*x + p01*y + p20*x*x + p11*x*y + p02*y*y + p21*x*x*y + p12*x*y*y + p03*y*y*y);
    }
    return cell;

  }

  return new DummyDerivedCa(this->mpSolver, this->mpZeroStimulus);

}

// Explicit instantiation
template class ICCFactory_Large2D_Neural<1>;
template class ICCFactory_Large2D_Neural<2>;
template class ICCFactory_Large2D_Neural<3>;