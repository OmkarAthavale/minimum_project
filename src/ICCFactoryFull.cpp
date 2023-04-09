#include "ICCFactoryFull.hpp"

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void ICCFactoryFull<ELEMENT_DIM, SPACE_DIM>::ReadLaplaceFile()
{
	std::ifstream inLaplaceInfo("projects/mesh/Stomach2D/rat_scaffold_32_32_2_2D_laplace_longi.txt");
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

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
AbstractCardiacCell* ICCFactoryFull<ELEMENT_DIM, SPACE_DIM>::CreateCardiacCellForTissueNode(Node<SPACE_DIM>* pNode)
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
  

  if (V_val > 40)
  {
    return new DummyDerivedCa(this->mpSolver, this->mpZeroStimulus);
  }

  if (counter != 1)
  {
    PRINT_4_VARIABLES(x,y,z, counter);
    EXCEPTION("Coordinates not found in Laplace file");
  }


  ChasteEllipsoid<SPACE_DIM> pacemaker(*pm_centre, *pm_radius);
  
  if(setICCNode.find(index) != setICCNode.end())
  {
	  CellICCSMCFromCellML* cell = new CellICCSMCFromCellML(this->mpSolver, this->mpZeroStimulus);
    
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
    // cell->SetParameter("eta", 0.037);
	double parMax = -65.0;
	double parMin = -72.0;
	
	// if ((V_val < 41) & (V_val > 37)) { cell->SetParameter("eta", 0.035);} else {
	cell->SetParameter("E_K", parMin+V_val*(parMax-parMin)/100.0);
		// };
	
    if (pacemaker.DoesContain(pNode->GetPoint()))
    {
		double scalingVal = (((pNode->GetPoint()[0]-pacemaker.rGetCentre()[0])/ pacemaker.rGetRadii()[0])+((pNode->GetPoint()[1]-pacemaker.rGetCentre()[1])/ pacemaker.rGetRadii()[0])+((pNode->GetPoint()[2]-pacemaker.rGetCentre()[2])/ pacemaker.rGetRadii()[0]))/3.0;
      cell->SetParameter("eta", 0.0380+0.009*scalingVal);
      
    }

    return cell;

  }

  return new DummyDerivedCa(this->mpSolver, this->mpZeroStimulus);

}

// Explicit instantiation
template class ICCFactoryFull<1>;
template class ICCFactoryFull<2>;
template class ICCFactoryFull<3>;
template class ICCFactoryFull<2, 3>;