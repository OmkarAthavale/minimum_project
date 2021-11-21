
#include "../src/ParamConfig.hpp"
#include "Debug.hpp"
TidyNeuralData::TidyNeuralData(std::string dataFile){
    std::ifstream in(dataFile.c_str());


    // if (in.is_open()) {
    //     // print file:
    //     char c = in.get();
    //     while (in.good()) {
    //     TRACE(c);
    //     c = in.get();
    //     }
    // }
    // else {
    //     // show message:
    //     TRACE("Error opening file");
    // }

    std::string line;
    std::string::size_type sz;     // alias of size_t

    // Row of times
    std::getline(in,line);       
    TRACE(line);
    times.push_back(std::stod(line,&sz));

    // Row of control regions
    std::getline(in,line);       
    TRACE(line);
    ctrlRegions.push_back(std::stoi(line,&sz));

    // Row of parameter names
    std::getline(in,line); 
    TRACE(line);
    std::stringstream ss(line);
    std::string item;

    while (getline (ss, item, ' ')) {
        paramNames.push_back (item);
        TRACE(item);
    }

    // Row of parameter values
    std::getline(in,line);       
    TRACE(line);
    paramVals.push_back(std::stod(line,&sz));

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
    return instance;
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

void ParamConfig::MapNodeToControl(DistributedTetrahedralMesh<2,2>& mesh){
    
    for (unsigned i=0; i<keyNum; ++i){
        std::vector<unsigned> nodes;
        for (DistributedTetrahedralMesh<2,2>::NodeIterator iter = mesh.GetNodeIteratorBegin(); iter != mesh.GetNodeIteratorEnd(); ++iter){
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

    while (time >= nextChangeTime){
        unsigned ctrlReg = NData.GetCtrlReg();

        std::vector<unsigned>::iterator it;

        for(it = nodeMapping.find(ctrlReg)->second.begin(); it != nodeMapping.find(ctrlReg)->second.end(); it++){
            changeNodes.push_back(NeuralChangeSet(*it, NData.GetParamName(), NData.GetParamVal()));
        }

        nextChangeTime = NData.NextTime();

    }
        
}

ParamConfig::ParamConfig(std::string NdataLoc):NData(NdataLoc){
    nextChangeTime = NData.NextTime();
}

double TidyNeuralData::NextTime(){
    currIndex++;
    return times[currIndex];
}

int TidyNeuralData::GetCtrlReg(){
    return ctrlRegions[currIndex];
}

std::string TidyNeuralData::GetParamName(){
    return paramNames[currIndex];
}

double TidyNeuralData::GetParamVal(){
    return paramVals[currIndex];
}

