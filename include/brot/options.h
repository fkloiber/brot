#ifndef BROT_OPTIONS_HEADER
#define BROT_OPTIONS_HEADER

#include <cstdio>

struct options_t
{
    bool double_precision = true;
    size_t width=100, height=100;
    size_t max_blocks;
    double error;
    double radius=2.0;
    double real_low=-2.0, real_high=2.0;
    double imag_low=-2.0, imag_high=2.0;
    size_t iter_low=0, iter_high=10;
    size_t block_size=4096;
    const char* filename;
    double use_max=false, use_err=false;
};

bool parse_options(options_t& opt, bool print, int argc, char **argv);

#endif//BROT_OPTIONS_HEADER
