#ifndef PARAMCONFIG_HPP_
#define PARAMCONFIG_HPP_

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

#include "../src/NeuralComponents.hpp"
#include "ChasteCuboid.hpp"
#include "ChastePoint.hpp"
#include "DistributedTetrahedralMesh.hpp"
#include "ChasteSerialization.hpp"

void split(const std::string &s, char delim, std::back_insert_iterator<std::vector<unsigned > > result);
void split(const std::string &s, char delim, std::back_insert_iterator<std::vector<double > > result);
void split(const std::string &s, char delim, std::back_insert_iterator<std::vector<std::string > > result);

class TidyNeuralData
{
    private:
    std::vector< double > times;
    std::vector< unsigned > ctrlRegions;
    std::vector< std::string > paramNames;
    std::vector< double > paramVals;
    unsigned currIndex = 0;
    std::size_t maxLength;

    public:
    TidyNeuralData(std::string dataFile);
    double NextTime();
    double GetInitTime();
    int GetCtrlReg();
    std::string GetParamName();
    double GetParamVal();

    bool neural_end = false;
};

struct NeuralChangeSet
{
    unsigned globalIndex;
    std::string paramName;
    double paramValue;

    NeuralChangeSet(unsigned index, std::string name, double value):globalIndex(index), paramName(name), paramValue(value){};
};

template <unsigned DIM>
class ParamConfig
{
    private:
    std::unordered_map<int, ChasteCuboid<DIM> > ctrlRegionDefn;
    std::unordered_map<unsigned, std::vector<unsigned>> nodeMapping;
    unsigned keyNum = 0;
    TidyNeuralData NData;
    static ParamConfig<DIM>* instance;
    double nextChangeTime = 999999999999.0;

    ParamConfig(std::string NdataLoc);

    public:
    static ParamConfig* InitInstance(std::string NdataLoc);
    static ParamConfig* GetInstance();

    void CreateGriddedControlRegions(double lb_x, double ub_x, int bins_x, double lb_y, double ub_y, int bins_y);
    void CreateGriddedControlRegions(double lb_x, double ub_x, int bins_x, double lb_y, double ub_y, int bins_y, double lb_z, double ub_z, int bins_z);
    void MapNodeToControl(AbstractTetrahedralMesh<2,2>* mesh);
    void MapNodeToControl(AbstractTetrahedralMesh<3,3>* mesh);
    void GetUpdateList(double time, std::vector<NeuralChangeSet> changeNodes);

};

#endif