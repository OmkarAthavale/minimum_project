#include "ICCFactoryFull.hpp"

template<unsigned DIM>
void ICCFactoryFull<DIM>::ReadLaplaceFile()
{
	std::ifstream inLaplaceInfo("projects/mesh/scaffold_full/rat_scaffold_16_16_2.1_laplace_longi_sw.txt");
	if(!inLaplaceInfo)
	{
	  EXCEPTION("Reading laplace solution error");
	}
	std::string line;
	coordinateV_st lapInfo;

	while(std::getline(inLaplaceInfo, line))
	{
	  std::stringstream cordinateLap(line);
	  cordinateLap >> lapInfo.x >> lapInfo.y >> lapInfo.z >> lapInfo.V;
	  LaplaceInfo.push_back(lapInfo);
	}
}

template<unsigned DIM>
AbstractCardiacCell* ICCFactoryFull<DIM>::CreateCardiacCellForTissueNode(Node<DIM>* pNode)
{
  double x = pNode->rGetLocation()[0];
  double y = pNode->rGetLocation()[1];
  double z = pNode->rGetLocation()[2];
  unsigned index = pNode->GetIndex();
  
  coordinateV_st info;
  int counter = 0;
  double V_val = 0;
  for(std::vector<coordinateV_st>::iterator itr = LaplaceInfo.begin(); itr!=LaplaceInfo.end();itr++)
  {
    info = *itr;
    if(info.x > x-0.001 && info.x < x+0.001  && info.y > y-0.001 && info.y < y+0.001 && info.z > z-0.001 && info.z < z + 0.001)
    {
      counter++;
      V_val = info.V;
      break;
    }
  }
  

  if (V_val > 97)
  {
    return new DummyDerivedCa(this->mpSolver, this->mpZeroStimulus);
  }
  

  ChasteEllipsoid<DIM> pacemaker(*pm_centre, *pm_radius);
  
  if(setICCNode.find(index) != setICCNode.end())
  {
	  
	  if 
	  
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
template class ICCFactoryFull<1>;
template class ICCFactoryFull<2>;
template class ICCFactoryFull<3>;