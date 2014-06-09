#include "stdafx.hpp"
#include "Genome.hpp"


namespace GridGALib
{
    std::size_t Genome::GenomeID = 0;

    Genome::Genome(void)
    :
        mGenomeID(++GenomeID),
        //mPnLPips(0),
        //mNumTrades(0),
        //mMedianTrade(0),
        //mMaxDrawDown(0),
       // mAccuracy(0),
        mComplete(false),
        //mSharpe(0),
        //mMSE(0.0),
        mObjective(0.0)
        //mObjectiveFunction(objectiveFunction)
    {
    }

    //______________________________________________________________________________________________________________

    void Genome::SetIntParameterList(GAParameterMapPtr parameters)
    {
        mParameters = parameters;
    }

    //______________________________________________________________________________________________________________

    void Genome::SetRandomValues()
    {
        BOOST_FOREACH(GAParameterMap::value_type& parameter, *mParameters)
        {
            parameter.second->SetRandomValue();
        }
    }

    //______________________________________________________________________________________________________________

    void Genome::LoadFromXML(const boost::property_tree::ptree& pt)
    {
        mGenomeID = pt.get("id", 0);

        BOOST_FOREACH(GAParameterMap::value_type& parameter, *mParameters)
        {
            if (parameter.second->GetHasValue())
            {
                parameter.second->SetInternalValue(pt.get(parameter.second->GetIdentifier(), 0));
            }
        }

        mObjective = pt.get("objective", 0.0);
        //mPnLPips = pt.get("num-pips", 0.0);
        //mNumTrades = pt.get("num-trades", 0);
        //mMedianTrade = pt.get("median-trade", 0.0);
        //mMaxDrawDown = pt.get("max-draw-down", 0.0);
        //mAccuracy = pt.get("accuracy", 0.0);
        mComplete = CommonLib::GetOptionalBoolParameter("complete", pt, false);
        //mSharpe = pt.get("sharpe", 0.0);
        //mMSE = pt.get("mse", 0.0);
        mComputeHost = pt.get("compute-host", "undefined");

        if (mGenomeID >= GenomeID)
        {
            GenomeID = mGenomeID + 1;
        }
    }

    //______________________________________________________________________________________________________________

 //   std::string Genome::GetConfigFileName(std::string subDir)
 //   {
 //       std::ostringstream s;
 //       s << "config." << mGenomeID << ".";
 //       return s.str();
 //   }

    //______________________________________________________________________________________________________________

    void Genome::Update(const boost::property_tree::ptree& pt)
    {
        // update the genome with the results of a backtest
        //mPnLPips = pt.get("results.num-pips", 0.0);
        //mNumTrades = pt.get("results.num-trades", 0);
        //mMedianTrade = pt.get("results.median-trade", 0.0);
        //mMaxDrawDown = pt.get("results.max-draw-down", 0.0);
        //mAccuracy = pt.get("results.accuracy", 0.0);
        //mSharpe = pt.get("results.sharpe", 0.0);
        //mMSE = pt.get("results.mse", 0.0);
        mObjective = pt.get("results.objective", 0.0);
        mComputeHost = pt.get("results.compute-host", "undefined");
        mComplete = true;
    }

    //______________________________________________________________________________________________________________

    void Genome::SaveAsXML(boost::property_tree::ptree& genomeTree) const
    {
        genomeTree.put("id", mGenomeID);

        BOOST_FOREACH(GAParameterMap::value_type& parameter, *mParameters)
        {
            if (parameter.second->GetHasValue())
            {
                genomeTree.put(parameter.second->GetIdentifier(), parameter.second->InternalValue());
            }
        }

        genomeTree.put("objective", mObjective);
        genomeTree.put("complete", mComplete ? "True" : "False");

        //genomeTree.put("num-pips", mPnLPips);
        //genomeTree.put("num-trades", mNumTrades);
        //genomeTree.put("median-trade", mMedianTrade);
        //genomeTree.put("max-draw-down", mMaxDrawDown);
        //genomeTree.put("accuracy", mAccuracy);
        //
        //genomeTree.put("sharpe", mSharpe);
        //genomeTree.put("mse", mMSE);
        //genomeTree.put("compute-host", mComputeHost);
    }

    //______________________________________________________________________________________________________________

    void Genome::SetInternalParameterValue(const std::string& parameterId, boost::int32_t value)
    {
        (*mParameters)[parameterId]->SetInternalValue(value);
    }

    //______________________________________________________________________________________________________________

    boost::int32_t Genome::GetInternalParameterValue(const std::string& parameterId)
    {
        return (*mParameters)[parameterId]->InternalValue();
    }

    //______________________________________________________________________________________________________________

    double Genome::GetObjective(void) const
    {
        if (!mComplete)
        {
            return -1.0 * std::numeric_limits<double>::max();
        }

        return mObjective;
    }

    //______________________________________________________________________________________________________________

    std::size_t Genome::GetGenomeID(void) const
    {
        return mGenomeID;
    }

    //______________________________________________________________________________________________________________
    // We have a 1 in %mutationProbability% of mutating
    bool Genome::Mutate(std::size_t mutationProbability)
    {
        // determine if this genome will be mutated
        if (rand() % mutationProbability != 0)
        {
            return false;
        }

        // mutation type either moves the gene one step, or substitutes a new random value
        boost::int32_t mutationType = (rand() % 2);

        boost::int32_t mutationPoint = rand() % ((*mParameters).size() - 1);
        GAParameterMap::iterator paramaterToMutate = (*mParameters).begin();
        std::advance(paramaterToMutate, mutationPoint);

        if (mutationType == 0)
        {
            boost::int32_t mutateDirection = (rand() % 2);

            if (mutateDirection == 0)
            {
                FILE_LOG(logDEBUG) << __FUNCTION_NAME__ << "- mutating " << paramaterToMutate->second->GetIdentifier() << " decrease";
                paramaterToMutate->second->Decrease();
            }
            else
            {
                FILE_LOG(logDEBUG) << __FUNCTION_NAME__ << "- mutating " << paramaterToMutate->second->GetIdentifier() << " increase";
                paramaterToMutate->second->Increase();
            }
            return true;
        }

        paramaterToMutate->second->SetRandomValue();
        FILE_LOG(logDEBUG) << __FUNCTION_NAME__ << "- mutating " << paramaterToMutate->second->GetIdentifier() << " to new random value of " <<
            paramaterToMutate->second->InternalValue();
        return true;
    }

    //______________________________________________________________________________________________________________

    std::string Genome::ToString(void) const
    {
        std::ostringstream s;
        //s << mGenomeID << ": Obj=" << GetObjective() << " Sharpe=" << mSharpe << " PnL=" << mPnLPips <<
        //    " dd=" << mMaxDrawDown << " num=" << mNumTrades << " " << "%=" << mAccuracy << " Compute host=" << mComputeHost << " ";

        s << mGenomeID << ": Obj=" << GetObjective() << " Compute host=" << mComputeHost << " ";

        BOOST_FOREACH(const GAParameterMap::value_type& parameter, *mParameters)
        {
            if (parameter.second->GetHasValue())
            {
                s << parameter.second->GetIdentifier() << "=" << parameter.second->GetValueForConfig() << " ";
            }
        }
        return s.str();
    }

    //______________________________________________________________________________________________________________

    GAParameterMapPtr Genome::GetParameters() const
    {
        return mParameters;
    }

    //______________________________________________________________________________________________________________

    bool Genome::IsComplete() const
    {
        return mComplete;
    }

    //______________________________________________________________________________________________________________

    std::string Genome::GetCommandLineArguments(const std::string& paramPrefix, const std::string& valuePrefix) const
    {
        std::ostringstream s;
        BOOST_FOREACH(const GAParameterMap::value_type& parameter, *mParameters)
        {
            if (parameter.second->GetHasValue())
            {
                s << paramPrefix << parameter.second->GetIdentifier() << valuePrefix << parameter.second->GetValueForConfig() << " ";
            }
        }
        return s.str();
    }

    //______________________________________________________________________________________________________________

    void Genome::SetGenerationNumber(boost::int32_t generationNumber)
    {
        GenomeID = generationNumber * 1000;
    }

    //______________________________________________________________________________________________________________
}
