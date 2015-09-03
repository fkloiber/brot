#ifndef BROT_OPTIONS_HEADER
#define BROT_OPTIONS_HEADER

#include <cstdio>
#include <string>

struct options_t
{
    bool double_precision = true;
    size_t run_size=2048;
    size_t block_size=1024;
    size_t max_blocks;
    double error;
    double radius=2.0;
    size_t width=100, height=100;
    double map_rlow=-2.0, map_rhigh=2.0;
    double map_ilow=-2.0, map_ihigh=2.0;
    double rand_rlow=-2.0, rand_rhigh=2.0;
    double rand_ilow=-2.0, rand_ihigh=2.0;
    size_t iter_low=0, iter_high=10;
    std::string filename;
    bool use_max=false, use_err=false;
    bool compress=false;
};

bool parse_options(options_t& opt, bool print, int argc, char **argv);

#endif//BROT_OPTIONS_HEADER
