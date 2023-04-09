#include "MonodomainProblemNeural.hpp"

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
MonodomainProblemNeural<ELEMENT_DIM, SPACE_DIM>::MonodomainProblemNeural(
            AbstractCardiacCellFactory<ELEMENT_DIM, SPACE_DIM>* pCellFactory)
    : MonodomainProblem<ELEMENT_DIM, SPACE_DIM>(pCellFactory)
{
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
MonodomainProblemNeural<ELEMENT_DIM, SPACE_DIM>::MonodomainProblemNeural()
    : MonodomainProblem<ELEMENT_DIM, SPACE_DIM>()
{
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
MonodomainProblemNeural<ELEMENT_DIM, SPACE_DIM>::~MonodomainProblemNeural()
{
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void MonodomainProblemNeural<ELEMENT_DIM, SPACE_DIM>::AtBeginningOfTimestep(double time)
{
  // Run electrode update as per BidomainProblem
  MonodomainProblem<ELEMENT_DIM, SPACE_DIM>::AtBeginningOfTimestep(time);

  // TODO: Update parameters from singleton instance of ParamConfig
  // -- Where parameters are changed in a region: for cells in region, set respective parameters to new value
  // ---- Call a function of ParamConfig that returns a vector of objects
  // ---- where each object has (int globalIndex, std::string paramNameString, double paramValue)
  // TRACE("-1");
  if (ParamConfig<ELEMENT_DIM, SPACE_DIM>::GetInstance() != NULL) {
    // TRACE("0");
    std::vector<NeuralChangeSet> changeNodes;
    // TRACE("1");
    ParamConfig<ELEMENT_DIM, SPACE_DIM>::GetInstance()->GetUpdateList(time, changeNodes);

    // TRACE("2:" << changeNodes.size());
    std::vector<NeuralChangeSet>::iterator row;

    // TRACE("3");
    for(row = changeNodes.begin(); row != changeNodes.end(); row++){
      try
      {
		double initVal = this->GetMonodomainTissue()->GetCardiacCell(row->globalIndex)->GetParameter(row->paramName);
        this->GetMonodomainTissue()->GetCardiacCell(row->globalIndex)->SetParameter(row->paramName, row->paramValue);
		double chVal = this->GetMonodomainTissue()->GetCardiacCell(row->globalIndex)->GetParameter(row->paramName);
		std::cerr << row->globalIndex << ", " << initVal << ", " << chVal << "\n";
      }
      catch (...)
      {
        std::cerr << "Skipped " << row->globalIndex << '\n';
      }
      
    }

  }
}


// Explicit instantiation
template class MonodomainProblemNeural<1>;
template class MonodomainProblemNeural<2>;
template class MonodomainProblemNeural<3>;
template class MonodomainProblemNeural<2, 3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS2(MonodomainProblemNeural, 1, 1)
EXPORT_TEMPLATE_CLASS2(MonodomainProblemNeural, 2, 3)
EXPORT_TEMPLATE_CLASS2(MonodomainProblemNeural, 2, 2)
EXPORT_TEMPLATE_CLASS2(MonodomainProblemNeural, 3, 3)