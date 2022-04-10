#include "MonodomainProblemNeural.hpp"

template<unsigned PROBLEM_DIM>
MonodomainProblemNeural<PROBLEM_DIM>::MonodomainProblemNeural(
            AbstractCardiacCellFactory<PROBLEM_DIM>* pCellFactory)
    : MonodomainProblem<PROBLEM_DIM>(pCellFactory)
{
}

template<unsigned PROBLEM_DIM>
MonodomainProblemNeural<PROBLEM_DIM>::MonodomainProblemNeural()
    : MonodomainProblem<PROBLEM_DIM>()
{
}

template<unsigned PROBLEM_DIM>
MonodomainProblemNeural<PROBLEM_DIM>::~MonodomainProblemNeural()
{
}

template<unsigned PROBLEM_DIM>
void MonodomainProblemNeural<PROBLEM_DIM>::AtBeginningOfTimestep(double time)
{
  // Run electrode update as per BidomainProblem
  MonodomainProblem<PROBLEM_DIM>::AtBeginningOfTimestep(time);

  // TODO: Update parameters from singleton instance of ParamConfig
  // -- Where parameters are changed in a region: for cells in region, set respective parameters to new value
  // ---- Call a function of ParamConfig that returns a vector of objects
  // ---- where each object has (int globalIndex, std::string paramNameString, double paramValue)
  // TRACE("-1");
  if (ParamConfig<PROBLEM_DIM>::GetInstance() != NULL) {
    // TRACE("0");
    std::vector<NeuralChangeSet> changeNodes;
    // TRACE("1");
    ParamConfig<PROBLEM_DIM>::GetInstance()->GetUpdateList(time, changeNodes);

    // TRACE("2:" << changeNodes.size());
    std::vector<NeuralChangeSet>::iterator row;

    // TRACE("3");
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
