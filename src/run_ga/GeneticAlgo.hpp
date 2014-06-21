#pragma once

#include "stdafx.hpp"

#include "Genome.hpp"
#include "HTCondor.hpp"

namespace GridGALib
{
    typedef boost::function<void (const GenomePtr parent1, const GenomePtr parent2, GenomePtr child1, GenomePtr child2)> CrossFunc;

	class GeneticAlgo
    {
    public:
        GeneticAlgo(std::string configTemplateFileName, zmq::context_t& zmqContext);
        ~GeneticAlgo(void);
        bool ReadConfig(void);
        void Evolve(void);
        void SendTestMessage(std::string machineName, std::string sendString);

        GAParameterMapPtr GetRandomGAParameters();

        static std::string GetExampleConfig(void);
    private:
        GenomeList mGenomeCache;
        std::size_t mPopulationSize;
        double mNumBreedersPercent;
        std::size_t mMinNumBreeders;
        std::size_t mMutationProbability; 
        std::size_t mNumNewRandomGenomes;
        std::size_t mGenerationNumber;       
        std::size_t mNumGenerations;
        boost::int32_t mCondorClusterID;
        GAParameterMapPtr mParameterMap;
        std::string mFilesLocation;
        boost::int32_t mGAPort;
        zmq::context_t& mZmqContext;
        std::size_t mTimeoutMinutes;
        std::size_t mPrintBestNum;
        bool mUsingRecordedSignals;
        std::string mCacheFile;
        CrossFunc mCross;
        GetGenomeConfigFunc mGetGenomeConfig;
        boost::scoped_ptr<HTCondor> mHTCondor;

        std::string GetConfigForGA(const GenomePtr genome, const std::string& dir);

        void ParseConfigBlocks(boost::property_tree::ptree& pt, const GAParameterMapPtr parameterMap);
        bool GAParametersOk(const GAParameterMapPtr parameters);
        GenomePtr CreateRandomGenome(void);
        void CrossBySlicing(const GenomePtr parent1, const GenomePtr parent2, GenomePtr child1, GenomePtr child2);
        void CrossBySwap(const GenomePtr parent1, const GenomePtr parent2, GenomePtr child1, GenomePtr child2);
        bool AddGenomeToPopulation(GenomeList genomesToTest, GenomePtr genome);
        void RemoveIncomleteGenomes(void);
        GenomeList NextGeneration(void);
        void SendString(void* socket, const std::string& sendString) const; 
        void StoreState(void) const;
        bool RestoreState(void);   
        void SortPopulation();     
    };
}