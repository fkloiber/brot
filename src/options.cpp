#include <brot/options.h>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <getopt.h>

void print_usage(FILE *f)
{
    fprintf(f, "Usage: brot [options] <filename>\n\n"
        "  -h, --help       print this page and exit\n"
        "  -p, --precision  'single' or 'double' precision [double]\n"
        "  -z, --block-size number of points a node processes at once\n"
        "  -d, --radius     escape radius\n"
        "  -m, --max-blocks maximum number of blocks per node\n"
        "  -e, --error      error threshold\n"
        "  -W, --width      pixel width of the output image\n"
        "  -H, --height     pixel height of the output image\n"
        "  -r, --real-low   lower cutoff on real axis\n"
        "  -s, --real-high  higher cutoff on real axis\n"
        "  -i, --imag-low   lower cutoff on imaginary axis\n"
        "  -j, --imag-high  higher cutoff on imaginary axis\n"
        "  -k, --iter-low   only plot orbits of length >= iter-low\n"
        "  -l, --iter-high  only plot orbits of length <= iter-high\n"
        "  <filename>       file to write image to\n");
}

const struct option options[] =
{
    {"help",      no_argument,       nullptr, 'h'},
    {"precision", required_argument, nullptr, 'p'},
    {"width",     required_argument, nullptr, 'W'},
    {"height",    required_argument, nullptr, 'H'},
    {"radius",    required_argument, nullptr, 'd'},
    {"real-low",  required_argument, nullptr, 'r'},
    {"real-high", required_argument, nullptr, 's'},
    {"imag-low",  required_argument, nullptr, 'i'},
    {"imag-high", required_argument, nullptr, 'j'},
    {"iter-low",  required_argument, nullptr, 'k'},
    {"iter-high", required_argument, nullptr, 'l'},
    {"block-size",required_argument, nullptr, 'z'},
    {nullptr,     0,                 nullptr,  0 }
};

bool size_t_optarg(size_t& val, bool print)
{
    size_t pos;
    std::string opt = optarg;
    try {
        val = std::stoull(opt, &pos);
    } catch(std::invalid_argument e) {
        if(print) {
            fprintf(stderr, "Error: Invalid option argument: %s\n", optarg);
            print_usage(stderr);
        }
        return false;
    } catch(std::out_of_range e) {
        if(print) {
            fprintf(stderr, "Error: Value out of range %s\n", optarg);
            print_usage(stderr);
        }
        return false;
    }
    if(pos != opt.size()) {
        if(print) {
            fprintf(stderr, "Error: Invalid option argument: %s\n", optarg);
            print_usage(stderr);
        }
        return false;
    }
    return true;
}
bool double_optarg(double& val, bool print)
{
    size_t pos;
    std::string opt = optarg;
    try {
        val = std::stod(opt, &pos);
    } catch(std::invalid_argument e) {
        if(print) {
            fprintf(stderr, "Error: Invalid option argument: %s\n", optarg);
            print_usage(stderr);
        }
        return false;
    } catch(std::out_of_range e) {
        if(print) {
            fprintf(stderr, "Error: Value out of range %s\n", optarg);
            print_usage(stderr);
        }
        return false;
    }
    if(pos != opt.size()) {
        if(print) {
            fprintf(stderr, "Error: Invalid option argument: %s\n", optarg);
            print_usage(stderr);
        }
        return false;
    }
    return true;
}

bool parse_options(options_t& opt, bool print, int argc, char **argv)
{
    int option, option_index;
    static const std::string dbl="double";
    static const std::string flt="float";
    static const std::string sgl="single";
    std::string prec;
    opterr = 0;
    while((option = getopt_long(argc, argv, ":hp:z:d:m:e:W:H:r:s:i:j:k:l:", options, &option_index)) != -1) {
        switch(option) {
            case 'h':
                if(print)
                    print_usage(stdout);
                return true;
            break;

            case 'p':
                prec = optarg;
                if(std::mismatch(prec.begin(), prec.end(), dbl.begin()).first == prec.end()) {
                    opt.double_precision = true;
                    break;
                } else if(std::mismatch(prec.begin(), prec.end(), flt.begin()).first == prec.end() ||
                          std::mismatch(prec.begin(), prec.end(), sgl.begin()).first == prec.end()) {
                    opt.double_precision = false;
                    break;
                }
                if(print)
                    fprintf(stderr, "Error: Unknown option argument: %s\n", optarg);
                return false;
            break;

            case 'W':
                if(!size_t_optarg(opt.width, print))
                    return false;
            break;

            case 'H':
                if(!size_t_optarg(opt.height, print))
                    return false;
            break;

            case 'd':
                if(!double_optarg(opt.radius, print))
                    return false;
            break;

            case 'm':
                if(!size_t_optarg(opt.max_blocks, print))
                    return false;
                opt.use_max = true;
            break;

            case 'e':
                if(!double_optarg(opt.error, print))
                    return false;
                opt.use_err = true;
            break;

            case 'r':
                if(!double_optarg(opt.real_low, print))
                    return false;
            break;

            case 's':
                if(!double_optarg(opt.real_high, print))
                    return false;
            break;

            case 'i':
                if(!double_optarg(opt.imag_low, print))
                    return false;
            break;

            case 'j':
                if(!double_optarg(opt.imag_high, print))
                    return false;
            break;

            case 'k':
                if(!size_t_optarg(opt.iter_low, print))
                    return false;
            break;

            case 'l':
                if(!size_t_optarg(opt.iter_high, print))
                    return false;
            break;

            case 'z':
                if(!size_t_optarg(opt.block_size, print))
                    return false;
            break;


            case ':':
                if(print) {
                    fprintf(stderr, "Error: Missing argument to option %c\n", optopt);
                    print_usage(stderr);
                }
                return false;
            break;

            case '?':
                if(print) {
                    fprintf(stderr, "Error: Unknown option %c\n", optopt);
                    print_usage(stderr);
                }
                return false;
            break;
        }
    }

    int pos_args = argc - optind;
    if(pos_args == 0) {
        if(print) {
            fprintf(stderr, "Error: Missing argument 'filename'\n");
            print_usage(stderr);
        }
        return false;
    } else if(pos_args > 1) {
        if(print) {
            fprintf(stderr, "Error: Too many positional arguments\n");
            print_usage(stderr);
        }
        return false;
    }
    if(print) {
        FILE *f = fopen(argv[optind], "a");
        if(!f) {
            fprintf(stderr, "Error: Cannot open file \"%s\"\n", argv[optind]);
            print_usage(stderr);
            return false;
        }
        fclose(f);
    }
    opt.filename=argv[optind];

    if(!opt.use_max && !opt.use_err) {
        fprintf(stderr, "Error: Either --max-blocks or --error must be specified\n");
        print_usage(stderr);
        return false;
    }

    return true;
}
