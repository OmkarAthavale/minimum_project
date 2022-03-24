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
    void MapNodeToControl(AbstractTetrahedralMesh<2,2>* mesh);
    void GetUpdateList(double time, std::vector<NeuralChangeSet> changeNodes);

};

class SinusoidalData
{
    struct SinParamSet
    {
        double a0;
        double a1;
        double b1;
        double w;

        SinParamSet(double a0, double a1, double b1, double w):a0(a0), a1(a1), b1(b1), w(w){};
    }

    private:
    std::vector<SinParamSet> inputParams; // ([reg; a0; a1; b1; w;])
    std::string varName;

    public:
    SinusoidalData(std::string dataFile, std::string varName);
};


class FittedSinusoidConfig
{
    private:
    static FittedSinusoidConfig* instance;
    std::unordered_map<int, ChasteCuboid<2> > ctrlRegionDefn;
    std::unordered_map<unsigned, std::vector<unsigned> > nodeMapping;
    unsigned keyNum = 0;
    std::vector<SinusoidalData> data;

    public:
    static FittedSinusoidConfig* InitInstance();
    static FittedSinusoidConfig* GetInstance();

    void GetData(){return data};

}

#endif