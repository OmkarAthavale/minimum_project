#ifndef ICCFACTORY_LARGE2D_NEURAL_HPP_
#define ICCFACTORY_LARGE2D_NEURAL_HPP_

#include <set>

#include "AbstractCardiacCell.hpp"
#include "AbstractCardiacCellFactory.hpp"
#include "ChastePoint.hpp"
#include "ChasteEllipsoid.hpp"
#include "../src/DummyDerivedCa.hpp"
#include "../src/Du2013_neural_sens.hpp"

template<unsigned DIM>
class ICCFactory_Large2D_Neural : public AbstractCardiacCellFactory<DIM>
{
  private:
  std::set<unsigned> setICCNode;
  ChastePoint<DIM>* v1;
  ChastePoint<DIM>* v2;

  public:
  ICCFactory_Large2D_Neural(std::set<unsigned> iccNodes, ChastePoint<DIM>* vertex1, ChastePoint<DIM>* vertex2) : 
  AbstractCardiacCellFactory<DIM>(), 
  setICCNode(iccNodes), 
  v1(vertex1), 
  v2(vertex2)
  {};

  // Destructor
  virtual ~ICCFactory_Large2D_Neural(){};

  AbstractCardiacCell* CreateCardiacCellForTissueNode(Node<DIM>* pNode);
};

#endif