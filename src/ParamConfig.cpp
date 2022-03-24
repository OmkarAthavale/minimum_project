
#include "../src/ParamConfig.hpp"
#include "Debug.hpp"

// Function from https://stackoverflow.com/a/236803 26 Nov 2021
void split(const std::string &s, char delim, std::back_insert_iterator<std::vector<unsigned > > result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = std::stoi(item);
    }
}

void split(const std::string &s, char delim, std::back_insert_iterator<std::vector<double > > result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = std::stod(item);
    }
}

void split(const std::string &s, char delim, std::back_insert_iterator<std::vector<std::string > > result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}


TidyNeuralData::TidyNeuralData(std::string dataFile){

    bool debugRead = false;

    // Read input file
    std::ifstream in(dataFile.c_str());
    std::string line;

    // ----- Parse input file: Four rows of single space separated elements -----
    // Row of times
    std::getline(in,line);       
    if (debugRead) {TRACE(line)};
    split(line, ' ', std::back_inserter(times));
    if (debugRead) for (std::vector<double >::iterator i = times.begin(); i != times.end(); ++i) cout << *i << ", \n";

    // Row of control regions
    std::getline(in,line);       
    if (debugRead) {TRACE(line)};
    split(line, ' ', std::back_inserter(ctrlRegions));
    if (debugRead) for (std::vector<unsigned >::iterator i = ctrlRegions.begin(); i != ctrlRegions.end(); ++i) cout << *i << ", \n";

    // Row of parameter names
    std::getline(in,line); 
    if (debugRead) {TRACE(line)};
    split(line, ' ', std::back_inserter(paramNames));
    if (debugRead) for (std::vector<std::string >::iterator i = paramNames.begin(); i != paramNames.end(); ++i) cout << *i << ", \n";

    // Row of parameter values
    std::getline(in,line);       
    if (debugRead) {TRACE(line)};
    split(line, ' ', std::back_inserter(paramVals));
    if (debugRead) for (std::vector<double >::iterator i = paramVals.begin(); i != paramVals.end(); ++i) cout << *i << ", \n";

    maxLength = times.size();
}


ParamConfig* ParamConfig::instance = 0;

ParamConfig* ParamConfig::InitInstance(std::string NdataLoc)
{
    if (instance == 0)
    {
        instance = new ParamConfig(NdataLoc);
    }

    return instance;
}

ParamConfig* ParamConfig::GetInstance()
{
    if (instance == 0) {
        return NULL;
    } else {
        return instance;
    }
}

void ParamConfig::CreateGriddedControlRegions(double lb_x, double ub_x, int bins_x, double lb_y, double ub_y, int bins_y){


    double x_bin_size = (ub_x-lb_x)/bins_x;
    double y_bin_size = (ub_y-lb_y)/bins_y;

    for (int i=0; i<bins_y; ++i){
        for (int j=0; j<bins_x; ++j){
            keyNum++;
            
            ChastePoint<2> top_corner(lb_x+x_bin_size*j, lb_y+y_bin_size*i);
            ChastePoint<2> bottom_corner(lb_x+x_bin_size*(j+1), lb_y+y_bin_size*(i+1));
            ChasteCuboid<2> reg(top_corner, bottom_corner);
            ctrlRegionDefn.insert({keyNum, reg});
            
        }
    }
}

void ParamConfig::CreateGriddedControlRegions(double lb_x, double ub_x, int bins_x, double lb_y, double ub_y, int bins_y, double lb_z, double ub_z, int bins_z){


    double x_bin_size = (ub_x-lb_x)/bins_x;
    double y_bin_size = (ub_y-lb_y)/bins_y;
    double z_bin_size = (ub_z-lb_z)/bins_z;

    for (int i=0; i<bins_y; ++i){
        for (int j=0; j<bins_x; ++j){
            for (int k=0; j<bins_z; ++k){
            keyNum++;
            
            ChastePoint<3> top_corner(lb_x+x_bin_size*j, lb_y+y_bin_size*i, lb_z+z_bin_size*k);
            ChastePoint<3> bottom_corner(lb_x+x_bin_size*(j+1), lb_y+y_bin_size*(i+1), lb_z+z_bin_size*(k+1));
            ChasteCuboid<3> reg(top_corner, bottom_corner);
            ctrlRegionDefn.insert({keyNum, reg});
            
        }
    }
}

void ParamConfig::MapNodeToControl(AbstractTetrahedralMesh<2,2>* mesh){
    
    for (unsigned i=1; i<=keyNum; ++i){
        std::vector<unsigned> nodes;
        for (DistributedTetrahedralMesh<2,2>::NodeIterator iter = mesh->GetNodeIteratorBegin(); iter != mesh->GetNodeIteratorEnd(); ++iter){
            if (ctrlRegionDefn.find(i)!=ctrlRegionDefn.end() && ctrlRegionDefn.find(i)->second.DoesContain(iter->GetPoint())){
                nodes.push_back(iter->GetIndex());
            }
        }
            nodeMapping.insert({i, nodes});

            cout << i << ": ";
            for (std::vector<unsigned>::iterator n = nodes.begin(); n != nodes.end(); n++) cout << *n << ", ";
            cout << '\n';
    }

}

void ParamConfig::MapNodeToControl(AbstractTetrahedralMesh<3,3>* mesh){
    
    for (unsigned i=1; i<=keyNum; ++i){
        std::vector<unsigned> nodes;
        for (DistributedTetrahedralMesh<3,3>::NodeIterator iter = mesh->GetNodeIteratorBegin(); iter != mesh->GetNodeIteratorEnd(); ++iter){
            if (ctrlRegionDefn.find(i)!=ctrlRegionDefn.end() && ctrlRegionDefn.find(i)->second.DoesContain(iter->GetPoint())){
                nodes.push_back(iter->GetIndex());
            }
        }
            nodeMapping.insert({i, nodes});

            cout << i << ": ";
            for (std::vector<unsigned>::iterator n = nodes.begin(); n != nodes.end(); n++) cout << *n << ", ";
            cout << '\n';
    }

}

void ParamConfig::GetUpdateList(double time, std::vector<NeuralChangeSet> changeNodes){
    
    while (!NData.neural_end && time >= nextChangeTime){
        unsigned ctrlReg = NData.GetCtrlReg();

        std::vector<unsigned>::iterator it;

        for(it = nodeMapping.find(ctrlReg)->second.begin(); it != nodeMapping.find(ctrlReg)->second.end(); it++){
            changeNodes.push_back(NeuralChangeSet(*it, NData.GetParamName(), NData.GetParamVal()));
        }

        nextChangeTime = NData.NextTime();

    }
        
}

ParamConfig::ParamConfig(std::string NdataLoc):NData(NdataLoc){
    nextChangeTime = NData.GetInitTime();
}

double TidyNeuralData::NextTime(){

    if (currIndex < (maxLength - 1)) {
        currIndex++;
        return times[currIndex];
    } else {
        neural_end = true;
        return 0.0;
    }
    
}

double TidyNeuralData::GetInitTime(){
    return times[currIndex];
}

int TidyNeuralData::GetCtrlReg(){
    return ctrlRegions[currIndex];
}

std::string TidyNeuralData::GetParamName(){
    return paramNames[currIndex];
}

double TidyNeuralData::GetParamVal(){
    if (GetParamName() == "excitatory_neural") {
        return CalibrationFunctions::Beta_Zhang2011(paramVals[currIndex]);
    } else if (GetParamName() == "inhibitory_neural") {
        return CalibrationFunctions::GBKmax_Kim2003(paramVals[currIndex]);
    } else {
        return paramVals[currIndex];
    }
}


SinusoidalData::SinusoidalData(std::string dataFile) : varName(varName) {
    bool debugRead = false;

    // Read input file
    std::ifstream in(dataFile.c_str());
    std::string line;

    while (std::getline(in, line)) {
        std::vector<double> rowData;
        std::vector<SinParamSet> sets;
        split(line, ' ', std::back_inserter(rowData))
        inputParams.push_back(SinParamSet(static_cast<unsigned> rowData[0], rowData[1], rowData[2], rowData[3], rowData[4]))
        
        if (debugRead) for (std::vector<double >::iterator i = rowData.begin(); i != rowData.end(); ++i) cout << *i << ", \n";
    }
}