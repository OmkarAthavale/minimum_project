#ifndef ICCFACTORY_HPP_
#define ICCFACTORY_HPP_

#include <set>

#include "AbstractCardiacCell.hpp"
#include "AbstractCardiacCellFactory.hpp"
#include "ChastePoint.hpp"
#include "ChasteEllipsoid.hpp"
#include "../src/DummyDerivedCa.hpp"
#include "../src/Du2013_CalibNeur.hpp"

template<unsigned DIM>
class ICCFactory : public AbstractCardiacCellFactory<DIM>
{
  private:
  std::set<unsigned> setICCNode;
  ChastePoint<DIM>* pm_centre;
  ChastePoint<DIM>* pm_radius;
  std::vector<coordinateV_st> LaplaceInfo;
  
  void ReadLaplaceFile();

  public:
  ICCFactory(std::set<unsigned> iccNodes, ChastePoint<DIM>* centre, ChastePoint<DIM>* radius) : 
  AbstractCardiacCellFactory<DIM>(), 
  setICCNode(iccNodes), 
  pm_centre(centre), 
  pm_radius(radius)
  {ReadLaplaceFile();};

  // Destructor
  virtual ~ICCFactory(){};

  AbstractCardiacCell* CreateCardiacCellForTissueNode(Node<DIM>* pNode);
};

#endif