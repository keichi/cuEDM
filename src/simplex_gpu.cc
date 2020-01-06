#include <arrayfire.h>

#include "simplex_gpu.h"

void SimplexGPU::predict(Timeseries &prediction, std::vector<float> &buffer,
                         const LUT &lut, const Timeseries &target, uint32_t E)
{
    const auto shift = (E - 1) * tau + Tp;

    buffer.resize(lut.n_rows());

    const af::array idx(lut.n_cols(), lut.n_rows(), lut.indices.data());
    const af::array dist(lut.n_cols(), lut.n_rows(), lut.distances.data());
    const af::array target_data(target.size(), target.data());

    const af::array tmp =
        af::moddims(target_data(idx + shift), lut.n_cols(), lut.n_rows());
    const af::array pred = af::sum(tmp * dist);

    pred.host(buffer.data());

    prediction = Timeseries(buffer);
}