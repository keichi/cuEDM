#include <iostream>
#include <memory>
#include <string>

#include "thirdparty/argh/argh.h"

#include "knn_kernel.hpp"
#include "knn_kernel_cpu.hpp"
#include "timer.hpp"

void usage(const std::string &app_name)
{
    std::string msg =
        app_name +
        ": GPU-accelerated Empirical Dynamic Modeling\n"
        "\n"
        "Usage:\n"
        "  cuEDM [OPTION...] FILE\n"
        "  -t, --tau arg   Lag (default: 1)\n"
        "  -e, --emax arg  Maximum embedding dimension (default: 20)\n"
        "  -k, --topk arg  Number of neighbors to find (default: 100)\n"
        "  -h, --help      Show help";

    std::cout << msg << std::endl;
}

int main(int argc, char *argv[])
{
    argh::parser cmdl({"-t", "--tau", "-e", "--emax", "-k", "--topk"});
    cmdl.parse(argc, argv);

    if (cmdl[{"-h", "--help"}]) {
        usage(cmdl[0]);
        return 0;
    }

    if (!cmdl(0)) {
        std::cerr << "No input file" << std::endl;
        usage(cmdl[0]);
        return 1;
    }

    std::string fname = cmdl[1];
    int tau;
    cmdl({"t", "tau"}, 1) >> tau;
    int E_max;
    cmdl({"e", "emax"}, 20) >> E_max;
    int top_k;
    cmdl({"k", "topk"}, 100) >> top_k;

    std::cout << "Reading input dataset from " << fname << std::endl;

    Timer timer_tot;
    timer_tot.start();

    Dataset ds(fname);

    timer_tot.stop();

    std::cout << ds.n_rows << " rows read in " << timer_tot.elapsed() << " [ms]"
              << std::endl;

    timer_tot.start();

    int lut_n = ds.n_rows - (E_max - 1) * tau;
    if (lut_n <= 0) {
        std::cerr << "E or tau is too large" << std::endl;
        return 1;
    }
    if (lut_n < top_k) {
        std::cerr << "k is too large" << std::endl;
        return 1;
    }

    std::unique_ptr<KNNKernel> kernel =
        std::unique_ptr<KNNKernel>(new KNNKernelCPU(E_max, tau, top_k));

    for (int i = 0; i < ds.n_cols; i++) {
        Timer timer;
        timer.start();

        kernel->load_column(ds, i);
        kernel->run();

        timer.stop();

        std::cout << "Computed LUT for column #" << i << " in "
                  << timer.elapsed() << " [ms]" << std::endl;
    }

    timer_tot.stop();

    std::cout << "Processed dataset in " << timer_tot.elapsed() << " [ms]"
              << std::endl;

    return 0;
}
