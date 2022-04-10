
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
    if(!in)
	{
        EXCEPTION("Reading neural data error");
	}
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

template <unsigned DIM>
ParamConfig<DIM>* ParamConfig<DIM>::instance = 0;

template <unsigned DIM>
ParamConfig<DIM>* ParamConfig<DIM>::InitInstance(std::string NdataLoc)
{
    if (instance == 0)
    {
        instance = new ParamConfig<DIM>(NdataLoc);
    }

    return instance;
}

template <unsigned DIM>
ParamConfig<DIM>* ParamConfig<DIM>::GetInstance()
{
    if (instance == 0) {
        return NULL;
    } else {
        return instance;
    }
}

template <unsigned DIM>
void ParamConfig<DIM>::CreateGriddedControlRegions(double lb_x, double ub_x, int bins_x, double lb_y, double ub_y, int bins_y){


    double x_bin_size = (ub_x-lb_x)/bins_x;
    double y_bin_size = (ub_y-lb_y)/bins_y;

    for (int i=0; i<bins_y; ++i){
        for (int j=0; j<bins_x; ++j){
            keyNum++;
            
            ChastePoint<DIM> top_corner(lb_x+x_bin_size*j, lb_y+y_bin_size*i);
            ChastePoint<DIM> bottom_corner(lb_x+x_bin_size*(j+1), lb_y+y_bin_size*(i+1));
            ChasteCuboid<DIM> reg(top_corner, bottom_corner);
            ctrlRegionDefn.insert({keyNum, reg});
            
        }
    }
}

template <unsigned DIM>
void ParamConfig<DIM>::CreateGriddedControlRegions(double lb_x, double ub_x, int bins_x, double lb_y, double ub_y, int bins_y, double lb_z, double ub_z, int bins_z){


    double x_bin_size = (ub_x-lb_x)/bins_x;
    double y_bin_size = (ub_y-lb_y)/bins_y;
    double z_bin_size = (ub_z-lb_z)/bins_z;

    for (int i=0; i<bins_y; ++i){
        for (int j=0; j<bins_x; ++j){
            for (int k=0; k<bins_z; ++k){
            keyNum++;
            
            ChastePoint<DIM> top_corner(lb_x+x_bin_size*j, lb_y+y_bin_size*i, lb_z+z_bin_size*k);
            ChastePoint<DIM> bottom_corner(lb_x+x_bin_size*(j+1), lb_y+y_bin_size*(i+1), lb_z+z_bin_size*(k+1));
			cout << lb_x+x_bin_size*j << ", " << lb_y+y_bin_size*i << ", " << lb_z+z_bin_size*k << "::" << lb_x+x_bin_size*(j+1) << ", " << lb_y+y_bin_size*(i+1) << ", " << lb_z+z_bin_size*(k+1) << "\n";
            ChasteCuboid<DIM> reg(top_corner, bottom_corner);
            ctrlRegionDefn.insert({keyNum, reg});
            
            }
        }
    }
}
template <unsigned DIM>
void ParamConfig<DIM>::MapNodeToControl(AbstractTetrahedralMesh<DIM,DIM>* mesh){
    
    for (unsigned i=1; i<=keyNum; ++i){
        std::vector<unsigned> nodes;
        for (typename DistributedTetrahedralMesh<DIM,DIM>::NodeIterator iter = mesh->GetNodeIteratorBegin(); iter != mesh->GetNodeIteratorEnd(); ++iter){
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

template <unsigned DIM>
void ParamConfig<DIM>::MapNodeToControl(AbstractTetrahedralMesh<DIM,DIM>* mesh, std::string filename, double start, double end, double width){
    // read laplace soln
	std::ifstream inLaplaceInfo(filename);
	if(!inLaplaceInfo)
	{
        EXCEPTION("Reading laplace solution error");
	}
	std::string line;
	coordinateV_st lapInfo;
    std::vector<coordinateV_st> LaplaceInfo;

	while(std::getline(inLaplaceInfo, line))
	{
        std::stringstream cordinateLap(line);
        cordinateLap >> lapInfo.x >> lapInfo.y >> lapInfo.z >> lapInfo.V;
        LaplaceInfo.push_back(lapInfo);
	}

    std::vector<double> centres;
    for (double m = start + width / 2; m < end; m += width){centres.push_back(m);}


    for (unsigned i=0; i<centres.size(); ++i){
        std::vector<unsigned> nodes;
        for (typename DistributedTetrahedralMesh<DIM,DIM>::NodeIterator iter = mesh->GetNodeIteratorBegin(); iter != mesh->GetNodeIteratorEnd(); ++iter){

            coordinateV_st info;
            int counter = 0;
            double V_val = 0;

            double x = iter->GetPoint()[0];
            double y = iter->GetPoint()[1];
            double z = iter->GetPoint()[2];

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

            if (counter != 1)
            {
                PRINT_4_VARIABLES(x,y,z, counter);
                EXCEPTION("Coordinates not found in Laplace file");
            }
            if (std::abs(V_val - centres[i]) < (width/2)) 
            {
                nodes.push_back(iter->GetIndex());
            }
        }

        nodeMapping.insert({i+1, nodes});

        cout << i+1 << ": ";
        for (std::vector<unsigned>::iterator n = nodes.begin(); n != nodes.end(); n++) cout << *n << ", ";
        cout << '\n';
    }
}

template <unsigned DIM>
void ParamConfig<DIM>::GetUpdateList(double time, std::vector<NeuralChangeSet>& changeNodes){
    TRACE("B0");
    while (!NData.neural_end && time >= nextChangeTime){
		TRACE("B0: " << time << ", " << nextChangeTime);
        unsigned ctrlReg = NData.GetCtrlReg();

        std::vector<unsigned>::iterator it;

        for(it = nodeMapping.find(ctrlReg)->second.begin(); it != nodeMapping.find(ctrlReg)->second.end(); it++){
            changeNodes.push_back(NeuralChangeSet(*it, NData.GetParamName(), NData.GetParamVal()));
        }
        TRACE(changeNodes.size());
        nextChangeTime = NData.NextTime();

    }
}

template <unsigned DIM>
ParamConfig<DIM>::ParamConfig(std::string NdataLoc):NData(NdataLoc){
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
