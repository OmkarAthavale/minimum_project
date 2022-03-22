//! @file
//!
//! This source file was generated from CellML by chaste_codegen version 0.9.2
//!
//! Model: imtiaz_2002
//!
//! Processed by chaste_codegen: https://github.com/ModellingWebLab/chaste-codegen
//!     (translator: chaste_codegen, model type: Normal)
//! on 2022-03-22 02:10:59
//!
//! <autogenerated>

#include "Du2013_CalibNeur.hpp"
#include <cmath>
#include <cassert>
#include <memory>
#include "Exception.hpp"
#include "OdeSystemInformation.hpp"
#include "RegularStimulus.hpp"
#include "HeartConfig.hpp"
#include "IsNan.hpp"
#include "MathsCustomFunctions.hpp"




    double CellDu2013_CalibNeurFromCellML::GetIntracellularCalciumConcentration()
    {
        return mStateVariables[1];
    }
    CellDu2013_CalibNeurFromCellML::CellDu2013_CalibNeurFromCellML(boost::shared_ptr<AbstractIvpOdeSolver> pSolver, boost::shared_ptr<AbstractStimulusFunction> pIntracellularStimulus)
        : AbstractCardiacCell(
                pSolver,
                6,
                0,
                pIntracellularStimulus)
    {
        // Time units: millisecond
        //
        this->mpSystemInfo = OdeSystemInformation<CellDu2013_CalibNeurFromCellML>::Instance();
        Init();
        
        this->mParameters[0] = 26.0; // (var_Membrane__C_m) [capacitance_units]
        this->mParameters[1] = 1.0; // (var_Membrane__Cor) [dimensionless]
        this->mParameters[2] = 0; // (var_neural__stim_cholinergic) [dimensionless]
        this->mParameters[3] = 0; // (var_neural__stim_nitrergic) [dimensionless]
        this->mParameters[4] = 120000.0; // (var_neural__stim_end) [time_units]
        this->mParameters[5] = 60000.0; // (var_neural__stim_start) [time_units]
    }

    CellDu2013_CalibNeurFromCellML::~CellDu2013_CalibNeurFromCellML()
    {
    }

    
    double CellDu2013_CalibNeurFromCellML::GetIIonic(const std::vector<double>* pStateVariables)
    {
        // For state variable interpolation (SVI) we read in interpolated state variables,
        // otherwise for ionic current interpolation (ICI) we use the state variables of this model (node).
        if (!pStateVariables) pStateVariables = &rGetStateVariables();
        const std::vector<double>& rY = *pStateVariables;
        double var_chaste_interface__Membrane__V_m = (mSetVoltageDerivativeToZero ? this->mFixedVoltage : rY[0]);
        // Units: voltage_units; Initial value: -70.5156
        double var_chaste_interface__intracellular_Ca__Ca_c = rY[1];
        // Units: millimolar; Initial value: 0.2886
        double var_chaste_interface__d_Na__d_Na = rY[2];
        // Units: dimensionless; Initial value: 0.0
        double var_chaste_interface__f_Na__f_Na = rY[3];
        // Units: dimensionless; Initial value: 0.9997
        
        const double var_Membrane__C_m_converted = 9.9999999999999995e-7 * mParameters[0]; // uF
        const double var_i_BK__E_K = -72.0; // voltage_units
        const double var_i_Ca__E_Ca = -20.0; // voltage_units
        const double var_i_Ca__G_MCa = 4.0; // conductance_units
        const double var_i_Ca__k_Ca = 0.93999999999999995; // millimolar
        const double var_i_Ca__q = 4.0; // dimensionless
        const double var_i_Na__E_Na = 80.0; // voltage_units
        const double var_i_Na__G_Na = 28.0; // conductance_units
        const double var_i_Na__I_Na = (-var_i_Na__E_Na + var_chaste_interface__Membrane__V_m) * var_chaste_interface__d_Na__d_Na * var_chaste_interface__f_Na__f_Na * var_i_Na__G_Na; // current_units
        const double var_d_BK__d_BK = 1 / (1.0 + exp(-13.815510557964274 - 2.0 * log(var_chaste_interface__intracellular_Ca__Ca_c) - 0.058823529411764705 * var_chaste_interface__Membrane__V_m)); // dimensionless
        const double var_i_Ca__G_Ca = pow(var_chaste_interface__intracellular_Ca__Ca_c, var_i_Ca__q) * var_i_Ca__G_MCa / (pow(var_i_Ca__k_Ca, var_i_Ca__q) + pow(var_chaste_interface__intracellular_Ca__Ca_c, var_i_Ca__q)); // conductance_units
        const double var_i_Ca__I_Ca = (-var_i_Ca__E_Ca + var_chaste_interface__Membrane__V_m) * var_i_Ca__G_Ca; // current_units
        const double var_neural__m = 1.3600000000000001; // conductance_units
        const double var_neural__n = 0.037699999999999997; // dimensionless
        const double var_neural__gbk_neur = var_neural__m * exp(var_neural__n * mParameters[3]); // conductance_units
        const double var_i_BK__I_BK = (-var_i_BK__E_K + var_chaste_interface__Membrane__V_m) * var_d_BK__d_BK * var_neural__gbk_neur; // current_units
        const double var_chaste_interface__i_ionic = 1.0000000000000002e-6 * (var_i_BK__I_BK + var_i_Ca__I_Ca + var_i_Na__I_Na) * HeartConfig::Instance()->GetCapacitance() / var_Membrane__C_m_converted; // uA_per_cm2

        const double i_ionic = var_chaste_interface__i_ionic;
        EXCEPT_IF_NOT(!std::isnan(i_ionic));
        return i_ionic;
    }

    void CellDu2013_CalibNeurFromCellML::EvaluateYDerivatives(double var_chaste_interface__Time__time, const std::vector<double>& rY, std::vector<double>& rDY)
    {
        // Inputs:
        // Time units: millisecond
        double var_chaste_interface__Membrane__V_m = (mSetVoltageDerivativeToZero ? this->mFixedVoltage : rY[0]);
        // Units: voltage_units; Initial value: -70.5156
        double var_chaste_interface__intracellular_Ca__Ca_c = rY[1];
        // Units: millimolar; Initial value: 0.2886
        double var_chaste_interface__d_Na__d_Na = rY[2];
        // Units: dimensionless; Initial value: 0.0
        double var_chaste_interface__f_Na__f_Na = rY[3];
        // Units: dimensionless; Initial value: 0.9997
        double var_chaste_interface__intracellular_Ca__Ca_s = rY[4];
        // Units: millimolar; Initial value: 2.0014
        double var_chaste_interface__intracellular_Ca__IP_3 = rY[5];
        // Units: millimolar; Initial value: 0.3791

        // Mathematics
        double d_dt_chaste_interface_var_Membrane__V_m;
        const double var_d_Na__d_inf_Na = 1 / (1.0 + exp(-1.3999999999999999 - 0.20000000000000001 * var_chaste_interface__Membrane__V_m)); // dimensionless
        const double var_d_Na__tau_d_Na = 10.26; // time_units
        const double d_dt_chaste_interface_var_d_Na__d_Na = (-var_chaste_interface__d_Na__d_Na + var_d_Na__d_inf_Na) * mParameters[1] / var_d_Na__tau_d_Na; // 1 / time_units
        const double var_f_Na__f_inf_Na = 1 / (1.0 + exp(9.3499999999999996 + 0.25 * var_chaste_interface__Membrane__V_m)); // dimensionless
        const double var_f_Na__tau_f_Na = 112.81999999999999; // time_units
        const double d_dt_chaste_interface_var_f_Na__f_Na = (-var_chaste_interface__f_Na__f_Na + var_f_Na__f_inf_Na) * mParameters[1] / var_f_Na__tau_f_Na; // 1 / time_units
        const double var_intracellular_Ca__K = 0.00064349999999999997; // per_time_units
        const double var_intracellular_Ca__P_MV = 0.032500000000000001; // millimolar_per_time_units
        const double var_intracellular_Ca__V_0 = 0.00011; // millimolar_per_time_units
        const double var_intracellular_Ca__V_1 = 0.00033; // per_time_units
        const double var_intracellular_Ca__V_M2 = 0.0048999999999999998; // millimolar_per_time_units
        const double var_intracellular_Ca__V_M3 = 0.32240000000000002; // millimolar_per_time_units
        const double var_intracellular_Ca__V_M4 = 0.00048749999999999998; // millimolar_per_time_units
        const double var_intracellular_Ca__V_in = var_chaste_interface__intracellular_Ca__IP_3 * var_intracellular_Ca__V_1 + var_intracellular_Ca__V_0; // millimolar_per_time_units
        const double var_intracellular_Ca__eta = 0.038899999999999997; // per_time_units
        const double var_intracellular_Ca__k_2 = 1.0; // millimolar
        const double var_intracellular_Ca__k_4 = 0.5; // millimolar
        const double var_intracellular_Ca__k_a = 0.90000000000000002; // millimolar
        const double var_intracellular_Ca__k_f = 5.8499999999999999e-5; // per_time_units
        const double var_intracellular_Ca__k_p = 0.65000000000000002; // millimolar
        const double var_intracellular_Ca__k_r = 2.0; // millimolar
        const double var_intracellular_Ca__k_v = -68.0; // voltage_units
        const double var_intracellular_Ca__m = 4.0; // dimensionless
        const double var_intracellular_Ca__n = 2.0; // dimensionless
        const double var_intracellular_Ca__V_2 = pow(var_chaste_interface__intracellular_Ca__Ca_c, var_intracellular_Ca__n) * var_intracellular_Ca__V_M2 / (pow(var_chaste_interface__intracellular_Ca__Ca_c, var_intracellular_Ca__n) + pow(var_intracellular_Ca__k_2, var_intracellular_Ca__n)); // millimolar_per_time_units
        const double var_intracellular_Ca__o = 4.0; // dimensionless
        const double var_intracellular_Ca__r = 5.0; // dimensionless
        const double var_intracellular_Ca__u = 4.0; // dimensionless
        const double var_intracellular_Ca__w = 4.0; // dimensionless
        const double var_intracellular_Ca__V_3 = pow(var_chaste_interface__intracellular_Ca__Ca_c, var_intracellular_Ca__w) * pow(var_chaste_interface__intracellular_Ca__Ca_s, var_intracellular_Ca__m) * pow(var_chaste_interface__intracellular_Ca__IP_3, var_intracellular_Ca__o) * var_intracellular_Ca__V_M3 / ((pow(var_chaste_interface__intracellular_Ca__Ca_c, var_intracellular_Ca__w) + pow(var_intracellular_Ca__k_a, var_intracellular_Ca__w)) * (pow(var_chaste_interface__intracellular_Ca__Ca_s, var_intracellular_Ca__m) + pow(var_intracellular_Ca__k_r, var_intracellular_Ca__m)) * (pow(var_chaste_interface__intracellular_Ca__IP_3, var_intracellular_Ca__o) + pow(var_intracellular_Ca__k_p, var_intracellular_Ca__o))); // millimolar_per_time_units
        const double d_dt_chaste_interface_var_intracellular_Ca__Ca_c = (-var_intracellular_Ca__V_2 + var_chaste_interface__intracellular_Ca__Ca_s * var_intracellular_Ca__k_f - var_chaste_interface__intracellular_Ca__Ca_c * var_intracellular_Ca__K + var_intracellular_Ca__V_3 + var_intracellular_Ca__V_in) * mParameters[1]; // millimolar / time_units
        const double d_dt_chaste_interface_var_intracellular_Ca__Ca_s = (-var_intracellular_Ca__V_3 - var_chaste_interface__intracellular_Ca__Ca_s * var_intracellular_Ca__k_f + var_intracellular_Ca__V_2) * mParameters[1]; // millimolar / time_units
        const double var_neural__i = 0.0041200000000000004; // millimolar_per_time_units
        const double var_neural__j = 0.0315; // dimensionless
        const double var_neural__k = -0.0040299999999999997; // millimolar_per_time_units
        const double var_neural__l = -0.38100000000000001; // dimensionless
        const double var_neural__beta_neur = var_neural__i * exp(var_neural__j * mParameters[2]) + var_neural__k * exp(var_neural__l * mParameters[2]); // millimolar_per_time_units
        const double d_dt_chaste_interface_var_intracellular_Ca__IP_3 = ((1.0 - pow(var_chaste_interface__Membrane__V_m, var_intracellular_Ca__r) / (pow(var_chaste_interface__Membrane__V_m, var_intracellular_Ca__r) + pow(var_intracellular_Ca__k_v, var_intracellular_Ca__r))) * var_intracellular_Ca__P_MV - var_chaste_interface__intracellular_Ca__IP_3 * var_intracellular_Ca__eta - pow(var_chaste_interface__intracellular_Ca__IP_3, var_intracellular_Ca__u) * var_intracellular_Ca__V_M4 / (pow(var_chaste_interface__intracellular_Ca__IP_3, var_intracellular_Ca__u) + pow(var_intracellular_Ca__k_4, var_intracellular_Ca__u)) + var_neural__beta_neur) * mParameters[1]; // millimolar / time_units

        if (mSetVoltageDerivativeToZero)
        {
            d_dt_chaste_interface_var_Membrane__V_m = 0.0;
        }
        else
        {
            const double var_Membrane__C_m_converted = 9.9999999999999995e-7 * mParameters[0]; // uF
            const double var_Membrane__I_stim_converted = -GetIntracellularAreaStimulus(var_chaste_interface__Time__time); // uA_per_cm2
            const double var_Membrane__I_stim = 999999.99999999988 * var_Membrane__C_m_converted * var_Membrane__I_stim_converted / HeartConfig::Instance()->GetCapacitance(); // current_units
            const double var_i_BK__E_K = -72.0; // voltage_units
            const double var_i_Ca__E_Ca = -20.0; // voltage_units
            const double var_i_Ca__G_MCa = 4.0; // conductance_units
            const double var_i_Ca__k_Ca = 0.93999999999999995; // millimolar
            const double var_i_Ca__q = 4.0; // dimensionless
            const double var_i_Na__E_Na = 80.0; // voltage_units
            const double var_i_Na__G_Na = 28.0; // conductance_units
            const double var_i_Na__I_Na = (-var_i_Na__E_Na + var_chaste_interface__Membrane__V_m) * var_chaste_interface__d_Na__d_Na * var_chaste_interface__f_Na__f_Na * var_i_Na__G_Na; // current_units
            const double var_d_BK__d_BK = 1 / (1.0 + exp(-13.815510557964274 - 2.0 * log(var_chaste_interface__intracellular_Ca__Ca_c) - 0.058823529411764705 * var_chaste_interface__Membrane__V_m)); // dimensionless
            const double var_i_Ca__G_Ca = pow(var_chaste_interface__intracellular_Ca__Ca_c, var_i_Ca__q) * var_i_Ca__G_MCa / (pow(var_i_Ca__k_Ca, var_i_Ca__q) + pow(var_chaste_interface__intracellular_Ca__Ca_c, var_i_Ca__q)); // conductance_units
            const double var_i_Ca__I_Ca = (-var_i_Ca__E_Ca + var_chaste_interface__Membrane__V_m) * var_i_Ca__G_Ca; // current_units
            const double var_neural__m = 1.3600000000000001; // conductance_units
            const double var_neural__n = 0.037699999999999997; // dimensionless
            const double var_neural__gbk_neur = var_neural__m * exp(var_neural__n * mParameters[3]); // conductance_units
            const double var_i_BK__I_BK = (-var_i_BK__E_K + var_chaste_interface__Membrane__V_m) * var_d_BK__d_BK * var_neural__gbk_neur; // current_units
            d_dt_chaste_interface_var_Membrane__V_m = -(-var_Membrane__I_stim + var_i_BK__I_BK + var_i_Ca__I_Ca + var_i_Na__I_Na) * mParameters[1] / mParameters[0]; // voltage_units / time_units
        }
        
        rDY[0] = d_dt_chaste_interface_var_Membrane__V_m;
        rDY[1] = d_dt_chaste_interface_var_intracellular_Ca__Ca_c;
        rDY[2] = d_dt_chaste_interface_var_d_Na__d_Na;
        rDY[3] = d_dt_chaste_interface_var_f_Na__f_Na;
        rDY[4] = d_dt_chaste_interface_var_intracellular_Ca__Ca_s;
        rDY[5] = d_dt_chaste_interface_var_intracellular_Ca__IP_3;
    }

    std::vector<double> CellDu2013_CalibNeurFromCellML::ComputeDerivedQuantities(double var_chaste_interface__Time__time, const std::vector<double> & rY)
    {
        // Inputs:
        // Time units: millisecond
        double var_chaste_interface__intracellular_Ca__Ca_c = rY[1];
        // Units: millimolar; Initial value: 0.2886
        
        // Mathematics
        const double var_Membrane__C_m_converted = 9.9999999999999995e-7 * mParameters[0]; // uF
        const double var_Membrane__I_stim_converted = -GetIntracellularAreaStimulus(var_chaste_interface__Time__time); // uA_per_cm2

        std::vector<double> dqs(4);
        dqs[0] = var_chaste_interface__Time__time;
        dqs[1] = var_chaste_interface__intracellular_Ca__Ca_c;
        dqs[2] = var_Membrane__C_m_converted;
        dqs[3] = var_Membrane__I_stim_converted;
        return dqs;
    }

template<>
void OdeSystemInformation<CellDu2013_CalibNeurFromCellML>::Initialise(void)
{
    this->mSystemName = "imtiaz_2002";
    this->mFreeVariableName = "Time__time";
    this->mFreeVariableUnits = "time_units";

    // rY[0]:
    this->mVariableNames.push_back("membrane_voltage");
    this->mVariableUnits.push_back("voltage_units");
    this->mInitialConditions.push_back(-70.5156);

    // rY[1]:
    this->mVariableNames.push_back("cytosolic_calcium_concentration");
    this->mVariableUnits.push_back("millimolar");
    this->mInitialConditions.push_back(0.2886);

    // rY[2]:
    this->mVariableNames.push_back("d_Na__d_Na");
    this->mVariableUnits.push_back("dimensionless");
    this->mInitialConditions.push_back(0.0);

    // rY[3]:
    this->mVariableNames.push_back("f_Na__f_Na");
    this->mVariableUnits.push_back("dimensionless");
    this->mInitialConditions.push_back(0.9997);

    // rY[4]:
    this->mVariableNames.push_back("intracellular_Ca__Ca_s");
    this->mVariableUnits.push_back("millimolar");
    this->mInitialConditions.push_back(2.0014);

    // rY[5]:
    this->mVariableNames.push_back("intracellular_Ca__IP_3");
    this->mVariableUnits.push_back("millimolar");
    this->mInitialConditions.push_back(0.3791);

    // mParameters[0]:
    this->mParameterNames.push_back("Membrane__C_m");
    this->mParameterUnits.push_back("capacitance_units");

    // mParameters[1]:
    this->mParameterNames.push_back("correction");
    this->mParameterUnits.push_back("dimensionless");

    // mParameters[2]:
    this->mParameterNames.push_back("excitatory_neural");
    this->mParameterUnits.push_back("dimensionless");

    // mParameters[3]:
    this->mParameterNames.push_back("inhibitory_neural");
    this->mParameterUnits.push_back("dimensionless");

    // mParameters[4]:
    this->mParameterNames.push_back("stim_end");
    this->mParameterUnits.push_back("time_units");

    // mParameters[5]:
    this->mParameterNames.push_back("stim_start");
    this->mParameterUnits.push_back("time_units");

    // Derived Quantity index [0]:
    this->mDerivedQuantityNames.push_back("Time__time");
    this->mDerivedQuantityUnits.push_back("time_units");

    // Derived Quantity index [1]:
    this->mDerivedQuantityNames.push_back("cytosolic_calcium_concentration");
    this->mDerivedQuantityUnits.push_back("millimolar");

    // Derived Quantity index [2]:
    this->mDerivedQuantityNames.push_back("membrane_capacitance");
    this->mDerivedQuantityUnits.push_back("uF");

    // Derived Quantity index [3]:
    this->mDerivedQuantityNames.push_back("membrane_stimulus_current");
    this->mDerivedQuantityUnits.push_back("uA_per_cm2");

    this->mInitialised = true;
}

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(CellDu2013_CalibNeurFromCellML)

