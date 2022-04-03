#ifndef ICCFACTORYFULL_HPP_
#define ICCFACTORYFULL_HPP_

#include <set>

#include "AbstractCardiacCell.hpp"
#include "AbstractCardiacCellFactory.hpp"
#include "ChastePoint.hpp"
#include "ChasteEllipsoid.hpp"
#include "../src/DummyDerivedCa.hpp"
#include "../src/Du2013_CalibNeur.hpp"

template<unsigned DIM>
class ICCFactoryFull : public AbstractCardiacCellFactory<DIM>
{
  private:
  std::set<unsigned> setICCNode;
  ChastePoint<DIM>* pm_centre;
  ChastePoint<DIM>* pm_radius;
  std::vector<coordinateV_st> LaplaceInfo;
  
  struct coordinateV_st
{
    double x;
    double y;
    double z;
    double V;
};

  void ReadLaplaceFile();

  public:
  ICCFactoryFull(std::set<unsigned> iccNodes, ChastePoint<DIM>* centre, ChastePoint<DIM>* radius) : 
  AbstractCardiacCellFactory<DIM>(), 
  setICCNode(iccNodes), 
  pm_centre(centre), 
  pm_radius(radius)
  {ReadLaplaceFile();};

  // Destructor
  virtual ~ICCFactoryFull(){};

  AbstractCardiacCell* CreateCardiacCellForTissueNode(Node<DIM>* pNode);
};

#endif