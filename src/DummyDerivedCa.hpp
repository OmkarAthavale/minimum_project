#ifndef DUMMYDERIVEDCA_HPP_
#define DUMMYDERIVEDCA_HPP_

//! @file
//! 
//! This source file was generated from CellML.
//! 
//! Model: dud_model
//! 
//! Processed by pycml - CellML Tools in Python
//!     (translators: 16469, pycml: 16306, optimize: 14997)
//! on Wed Jan 30 15:07:23 2013
//! 
//! <autogenerated>

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include "AbstractCardiacCell.hpp"
#include "AbstractStimulusFunction.hpp"

class DummyDerivedCa : public AbstractCardiacCell
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCardiacCell >(*this);
    }
    
    // 
    // Settable parameters and readable variables
    // 
    double var_chaste_interface__ICC_Membrane__Ca_i;
    double var_ICC_Membrane__Cm;
    double var_chaste_interface__i_ionic;
    
public:

    double Get_chaste_interface__ICC_Membrane__Ca_i();
    double Get_ICC_Membrane__Cm();
    double Get_chaste_interface__i_ionic();
    DummyDerivedCa(boost::shared_ptr<AbstractIvpOdeSolver> pSolver, boost::shared_ptr<AbstractStimulusFunction> pIntracellularStimulus);
    ~DummyDerivedCa();
    void VerifyStateVariables();
    double GetIIonic(const std::vector<double>* pStateVariables=NULL);
    void EvaluateYDerivatives(double var_chaste_interface__Time__time, const std::vector<double>& rY, std::vector<double>& rDY);
    std::vector<double> ComputeDerivedQuantities(double var_chaste_interface__Time__time, const std::vector<double> & rY);

};


// Needs to be included last
#include "SerializationExportWrapper.hpp"
CHASTE_CLASS_EXPORT(DummyDerivedCa)

namespace boost
{
    namespace serialization
    {
        template<class Archive>
        inline void save_construct_data(
            Archive & ar, const DummyDerivedCa * t, const unsigned int fileVersion)
        {
            const boost::shared_ptr<AbstractIvpOdeSolver> p_solver = t->GetSolver();
            const boost::shared_ptr<AbstractStimulusFunction> p_stimulus = t->GetStimulusFunction();
            ar << p_solver;
            ar << p_stimulus;
        }
        
        template<class Archive>
        inline void load_construct_data(
            Archive & ar, DummyDerivedCa * t, const unsigned int fileVersion)
        {
            boost::shared_ptr<AbstractIvpOdeSolver> p_solver;
            boost::shared_ptr<AbstractStimulusFunction> p_stimulus;
            ar >> p_solver;
            ar >> p_stimulus;
            ::new(t)DummyDerivedCa(p_solver, p_stimulus);
        }
        
    }
    
}

#endif // DummyDerivedCa_HPP_
