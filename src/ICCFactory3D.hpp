#ifndef ICCFACTORY3D_HPP_
#define TESTMINIMAL3D_HPP_

#define PROBLEM_SPACE_DIM 3

#include <set>

#include "AbstractCardiacCell.hpp"
#include "AbstractCardiacCellFactory.hpp"
#include "ChastePoint.hpp"
#include "ChasteEllipsoid.hpp"
#include "../src/DummyDerivedCa.hpp"
#include "../src/Du2013_neural.hpp"

class ICCFactory3D : public AbstractCardiacCellFactory<PROBLEM_SPACE_DIM>
{
  private:
  std::set<unsigned> setICCNode;
  ChastePoint<PROBLEM_SPACE_DIM>* pm_centre;
  ChastePoint<PROBLEM_SPACE_DIM>* pm_radius;

  public:
  ICCFactory3D(std::set<unsigned> iccNodes, ChastePoint<PROBLEM_SPACE_DIM>* centre, ChastePoint<PROBLEM_SPACE_DIM>* radius) : 
  AbstractCardiacCellFactory<PROBLEM_SPACE_DIM>(), 
  setICCNode(iccNodes), 
  pm_centre(centre), 
  pm_radius(radius)
  {};

  // Destructor
  virtual ~ICCFactory3D(){};

  AbstractCardiacCell* CreateCardiacCellForTissueNode(Node<PROBLEM_SPACE_DIM>* pNode);
};

#endif