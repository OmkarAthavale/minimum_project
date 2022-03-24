#ifndef MONODOMAINPROBLEMNEURAL_HPP_
#define MONODOMAINPROBLEMNEURAL_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

#include "MonodomainProblem.hpp"
#include "AbstractCardiacCellFactory.hpp"
#include "ParamConfig.hpp"

/**
 * Class which specifies and solves a monodomain problem.
 *
 * The solution vector is of the form:
 * (V_1, phi_1, V_2, phi_2, ......, V_N, phi_N),
 * where V_j is the voltage at node j and phi_j is the
 * extracellular potential at node j.
 */
template<unsigned DIM>
class MonodomainProblemNeural : public MonodomainProblem<DIM>
{
    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Serialize this class
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object< MonodomainProblem<DIM> >(*this);
    }
public:
    /**
     * Constructor
     * @param pCellFactory User defined cell factory which shows how the pde should
     *   create cells.
     */
    MonodomainProblemNeural(AbstractCardiacCellFactory<DIM>* pCellFactory);

    /**
     * Constructor just used for archiving
     */
    MonodomainProblemNeural();

    /**
     * Destructor
     */
     ~MonodomainProblemNeural();

    /**
     * Called at beginning of each time step in the main time-loop in
     * BidomainProblem::Solve() to switch on the electrodes (if there are any). 
     * Overloaded here to also update parameter values from neural data input.
     *
     * @param time  the current time
     */
    void AtBeginningOfTimestep(double time);

};

#include "SerializationExportWrapper.hpp" // Must be last
EXPORT_TEMPLATE_CLASS_SAME_DIMS(MonodomainProblemNeural)

#endif /*MonodomainProblemNeural_HPP_*/
