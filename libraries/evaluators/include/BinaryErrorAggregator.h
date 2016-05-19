////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryErrorAggregator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>

namespace evaluators
{
    class BinaryErrorAggregator
    {
    public:

        BinaryErrorAggregator() = default;
        BinaryErrorAggregator(const BinaryErrorAggregator&) = delete;
        BinaryErrorAggregator(BinaryErrorAggregator&&) = default;

        void Update(double prediction, double label, double weight);

    private:
        double _sumWeightedTruePositives = 0.0;
        double _sumWeightedTrueNegatives = 0.0;
        double _sumWeightedFalsePositives = 0.0;
        double _sumWeightedFalseNegatives = 0.0;
    };
}