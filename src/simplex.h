#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include "dataset.h"
#include "lut.h"

class Simplex
{
public:
    Simplex(int tau, int Tp, bool verbose) : tau(tau), Tp(Tp), verbose(verbose)
    {
    }
    virtual ~Simplex(){};

    // Predicted timeseries using Simplex projection. Note that this class
    // owns the underlying buffer of the prediction result (i.e. prediction
    // cannot outlive Simplex). Furthermore, calling predict() will overwrite
    // the prediction result generated by the previous call to predict(). LUT
    // needs to be pre-computed using NearestNeighbors.
    virtual void predict(Timeseries &prediction, const LUT &lut,
                         const Timeseries &target, int E) = 0;

    // Shift and trim the target timeseries so that its time index matches the
    // predicted timeseries.
    virtual void adjust_target(Timeseries &adjusted_target,
                               const Timeseries &target, int E)
    {
        const auto offset = (E - 1) * tau + Tp;
        const auto n_prediction = target.size() - offset;

        adjusted_target = Timeseries(target.data() + offset, n_prediction);
    }

protected:
    // Lag
    const int tau;
    // How many steps to predict in future
    const int Tp;
    // Enable verbose logging
    const bool verbose;
    // Minimum weight
    const float min_weight = 1e-6f;
};

#endif
