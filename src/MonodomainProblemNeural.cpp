#include "MonodomainProblemNeural.hpp"

template<unsigned DIM>
MonodomainProblemNeural<DIM>::MonodomainProblemNeural(
            AbstractCardiacCellFactory<DIM>* pCellFactory, bool hasBath)
    : MonodomainProblem<DIM>(pCellFactory, hasBath)
{
}

template<unsigned DIM>
MonodomainProblemNeural<DIM>::MonodomainProblemNeural()
    : MonodomainProblem<DIM>()
{
}

template<unsigned DIM>
MonodomainProblemNeural<DIM>::~MonodomainProblemNeural()
{
}

template<unsigned DIM>
void MonodomainProblemNeural<DIM>::AtBeginningOfTimestep(double time)
{
  // Run electrode update as per BidomainProblem
  MonodomainProblem<DIM>::AtBeginningOfTimestep(time);

  // TODO: Update parameters from singleton instance of ParamConfig
  // -- Where parameters are changed in a region: for cells in region, set respective parameters to new value
  // ---- Call a function of ParamConfig that returns a vector of objects
  // ---- where each object has (int globalIndex, std::string paramNameString, double paramValue)

  if (ParamConfig::GetInstance() != NULL) {
    std::vector<NeuralChangeSet> changeNodes;
    ParamConfig::GetInstance()->GetUpdateList(time, changeNodes);

    std::vector<NeuralChangeSet>::iterator row;

    for(row = changeNodes.begin(); row != changeNodes.end(); row++){
      this->GetMonodomainTissue()->GetCardiacCell(row->globalIndex)->SetParameter(row->paramName, row->paramValue);
    }

  }
}


// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(MonodomainProblemNeural)

// Explicit instantiation
template class MonodomainProblemNeural<1>;
template class MonodomainProblemNeural<2>;
template class MonodomainProblemNeural<3>;
