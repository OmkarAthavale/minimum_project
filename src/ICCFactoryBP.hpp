#ifndef ICCFACTORYBP_HPP_
#define ICCFACTORYBP_HPP_

#include <set>

#include "Debug.hpp"
#include "AbstractCardiacCell.hpp"
#include "AbstractCardiacCellFactory.hpp"
#include "ChastePoint.hpp"
#include "ChasteEllipsoid.hpp"
#include "../src/DummyDerivedCa.hpp"
#include "../src/ICC_SMC_BP.hpp"

#ifndef LAPLACESTRUCT
#define LAPLACESTRUCT
struct coordinateV_st
{
    double x;
    double y;
    double z;
    double V;
};

#endif //LAPLACESTRUCT
template<unsigned ELEMENT_DIM, unsigned SPACE_DIM = ELEMENT_DIM>
class ICCFactoryBP : public AbstractCardiacCellFactory<ELEMENT_DIM, SPACE_DIM>
{
  private:
  std::set<unsigned> setICCNode;
  ChastePoint<SPACE_DIM>* pm_centre;
  ChastePoint<SPACE_DIM>* pm_radius;
  std::vector<coordinateV_st> LaplaceInfo;
  


  void ReadLaplaceFile();

  public:
  ICCFactoryBP(std::set<unsigned> iccNodes, ChastePoint<SPACE_DIM>* centre, ChastePoint<SPACE_DIM>* radius) : 
  AbstractCardiacCellFactory<ELEMENT_DIM, SPACE_DIM>(), 
  setICCNode(iccNodes), 
  pm_centre(centre), 
  pm_radius(radius)
  {ReadLaplaceFile();};

  // Destructor
  virtual ~ICCFactoryBP(){};

  AbstractCardiacCell* CreateCardiacCellForTissueNode(Node<SPACE_DIM>* pNode);
};

#endif