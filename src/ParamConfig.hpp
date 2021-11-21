#ifndef PARAMCONFIG_HPP_
#define PARAMCONFIG_HPP_

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>


#include "ChasteCuboid.hpp"
#include "ChastePoint.hpp"
#include "DistributedTetrahedralMesh.hpp"
#include "ChasteSerialization.hpp"

class TidyNeuralData
{
    private:
    std::vector< double > times;
    std::vector< int > ctrlRegions;
    std::vector< std::string > paramNames;
    std::vector< double > paramVals;
    unsigned currIndex = -1;

    public:
    TidyNeuralData(std::string dataFile);
    double NextTime();
    int GetCtrlReg();
    std::string GetParamName();
    double GetParamVal();
};

struct NeuralChangeSet
{
    unsigned globalIndex;
    std::string paramName;
    double paramValue;

    NeuralChangeSet(unsigned index, std::string name, double value):globalIndex(index), paramName(name), paramValue(value){};
};

class ParamConfig
{
    private:
    std::unordered_map<int, ChasteCuboid<2> > ctrlRegionDefn;
    std::unordered_map<unsigned, std::vector<unsigned>> nodeMapping;
    unsigned keyNum = 0;
    TidyNeuralData NData;
    static ParamConfig* instance;
    double nextChangeTime = 999999999999.0;

    ParamConfig(std::string NdataLoc);

    public:
    static ParamConfig* InitInstance(std::string NdataLoc);
    static ParamConfig* GetInstance();

    void CreateGriddedControlRegions(double lb_x, double ub_x, int bins_x, double lb_y, double ub_y, int bins_y);
    void MapNodeToControl(DistributedTetrahedralMesh<2,2>& mesh);
    void GetUpdateList(double time, std::vector<NeuralChangeSet> changeNodes);

};

#endif