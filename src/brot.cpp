#include <cstdio>
#include <cinttypes>
#include <algorithm>
#include <type_traits>
#include <vector>

#include <mpi.h>
#include <omp.h>

#include <brot/options.h>
#include <brot/rand.h>
#include <brot/iterate.h>

template<class Floating, class Integer>
void iterate(const options_t& options, std::vector<uint32_t>& img);

void split_time(double time, int& hours, int& minutes, int& seconds, int& hund);
void print_info(double elapsed, uint64_t blocks, uint64_t maxblocks, uint64_t orbits, uint64_t points);

int main(int argc, char **argv)
{
    MPI_Init(nullptr, nullptr);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    options_t options;
    if(!parse_options(options, world_rank==0, argc, argv))
        return 1;

    if(world_rank == 0) {
        printf("Calculating buddhabrot %"PRIu64"x%"PRIu64" image with a "
               "run length of %"PRIu64" and a block size of %"PRIu64", "
               "considering a total of %"PRIu64" orbits per block\n",
               options.width, options.height, options.run_size, options.block_size,
               options.run_size*options.block_size);
        printf("The calculated will continue until ");
        if(options.use_max) {
            printf("%"PRIu64" blocks have been processed", options.max_blocks);
            if(options.use_err)
                printf(", or ");
            else
                printf("\n");
        }
        if(options.use_err)
            printf("the average error falls under %f\n", options.error);
        printf("An orbit will be included in the image if it escapes the "
               "circle of raduis %f between %"PRIu64" and %"PRIu64" iterations\n",
               options.radius, options.iter_low, options.iter_high);
        printf("Random points are drawn from [%f,%f]+i[%f,%f]\n",
               options.rand_rlow, options.rand_rhigh, options.rand_ilow, options.rand_ihigh);
        printf("The image is mapped from the region [%f,%f]+i[%f,%f]\n",
               options.map_rlow, options.map_rhigh, options.map_ilow, options.map_ihigh);
    }

    const size_t image_size = options.width*options.height;

    std::vector<uint32_t> img;
    if(options.double_precision) {
        iterate<double, uint64_t>(options, img);
    } else {
        iterate<float,  uint32_t>(options, img);
    }

    if(world_rank == 0) {
        FILE *f = fopen(options.filename, "wb");
        if(!f) {
            fprintf(stderr, "Error: Couldn't open file \"%s\" for writing\n", options.filename);
        } else {
            fwrite(img.data(), sizeof(uint32_t), image_size, f);
            fclose(f);
        }
    }

    MPI_Finalize();
    return 0;
}



template<class Floating>
inline size_t reduce_align(size_t s)
{
    if(std::is_same<Floating, float>::value)
        return (s & 7) == 0 ? s : (s-8) & ~7;
    else
        return (s & 3) == 0 ? s : (s-4) & ~3;
}

template<class Floating, class Integer>
void iterate(const options_t& options, std::vector<uint32_t>& img)
{
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    const int threads = omp_get_max_threads();
    const size_t image_size = options.width * options.height;
    const size_t rs = options.run_size;
    const Floating rl = options.map_rlow;
    const Floating rh = options.map_rhigh;
    const Floating il = options.map_ilow;
    const Floating ih = options.map_ihigh;
    const Floating rL = options.rand_rlow;
    const Floating rH = options.rand_rhigh;
    const Floating iL = options.rand_ilow;
    const Floating iH = options.rand_ihigh;
    img.resize(image_size);

    std::vector<Floating> points(4*threads*rs);
    std::vector<Integer>  escape(  threads*rs);
    std::vector<size_t>      idx(  threads*rs);
    std::vector<xorshift1024_t> rngs(threads);

    for(auto& rng : rngs) seed_xorshift1024_urandom(&rng);

    uint64_t orbits_written=0, points_written=0, blocks_written=0;
    double start_time = MPI_Wtime();
    if(mpi_rank == 0)
        print_info(0.0, 0, options.max_blocks, 0, 0);

    for(blocks_written=0; blocks_written < options.max_blocks; ++blocks_written) {
        if(mpi_rank != 0)
            memset(img.data(), 0, sizeof(uint32_t)*image_size);

        //#pragma omp parallel for
        for(size_t i = 0; i < options.block_size; ++i) {
            int thread_num = omp_get_thread_num();
            Floating *cr = points.data() + rs*(thread_num +  0);
            Floating *ci = points.data() + rs*(thread_num +  4);
            Floating *tr = points.data() + rs*(thread_num +  8);
            Floating *ti = points.data() + rs*(thread_num + 12);
            Integer  *im = escape.data() + rs* thread_num;
            size_t   *ix = idx.data()    + rs* thread_num;

            fill_uniform1024(cr, rs, rL, rH, &rngs[thread_num]);
            fill_uniform1024(ci, rs, iL, iH, &rngs[thread_num]);

            std::iota(ix, ix+rs, 0);
            bulb_test(cr, ci, im, rs);
            const size_t* bulb = std::partition(ix, ix+rs, [im](size_t x) {
                return im[x] == 0;
            });

            const size_t bulb_size = bulb - ix;
            const size_t bulb_block = reduce_align<Floating>(bulb_size);

            for(size_t j = 0; j < bulb_block; ++j) {
                tr[j] = cr[ix[j]];
                ti[j] = ci[ix[j]];
            }

            std::iota(ix, ix+bulb_block, 0);
            escape_test(tr, ti, im, bulb_block, options.iter_low, options.radius);
            const size_t* low = std::partition(ix, ix+bulb_block, [im, &options](size_t a) {
                return im[a] == options.iter_low;
            });

            const size_t low_size = low - ix;
            const size_t low_block = reduce_align<Floating>(low_size);

            for(size_t j = 0; j < low_block; ++j) {
                cr[j] = tr[ix[j]];
                ci[j] = ti[ix[j]];
            }

            std::iota(ix, ix+low_block, 0);
            escape_test(cr, ci, im, low_block, options.iter_high+1, options.radius);

            size_t* high = std::partition(ix, ix+low_block, [im, &options](size_t a) {
                return im[a] <= options.iter_high;
            });

            const size_t high_size = high - ix;
            const size_t high_block = reduce_align<Floating>(high_size);

            for(size_t j = 0; j < high_block; ++j) {
                tr[j] = cr[ix[j]];
                ti[j] = ci[ix[j]];
            }

            size_t t = write_orbits(tr, ti, high_block, options.iter_high,
                rl, rh, il, ih, img.data(), options.width, options.height);

            #pragma omp atomic
            points_written += t;
            #pragma omp atomic
            orbits_written += high_block;
        }
        if(mpi_rank == 0) {
            MPI_Reduce(MPI_IN_PLACE, img.data(),      image_size, MPI_UINT32_T, MPI_SUM, 0, MPI_COMM_WORLD);
            MPI_Reduce(MPI_IN_PLACE, &points_written, 1,          MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);
            MPI_Reduce(MPI_IN_PLACE, &orbits_written, 1,          MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);

            double current_time = MPI_Wtime();
            print_info(current_time-start_time, blocks_written+1, options.max_blocks,
                orbits_written, points_written);
        } else {
            MPI_Reduce(img.data(),      img.data(),      image_size, MPI_UINT32_T, MPI_SUM, 0, MPI_COMM_WORLD);
            MPI_Reduce(&points_written, &points_written, 1,          MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);
            MPI_Reduce(&orbits_written, &orbits_written, 1,          MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);
        }
    }
}

void split_time(double time, int& hours, int& minutes, int& seconds, int& hund)
{
    hund    = 100.0*time;
    seconds =    hund / 100;
    hund    =    hund % 100;
    minutes = seconds /  60;
    seconds = seconds %  60;
    hours   = minutes /  60;
    minutes = minutes %  60;
}

void print_info(double elapsed, uint64_t blocks, uint64_t maxblocks, uint64_t orbits, uint64_t points)
{
    int eh, em, es, ed, rh, rm, rs, rd;
    double progress = (double)blocks/maxblocks;
    double remaining = 0.0;
    if(blocks != 0) {
        remaining = (1.0-progress)/progress * elapsed;
    }

    split_time(elapsed, eh, em, es, ed);
    split_time(remaining, rh, rm, rs, rd);

    printf("runtime: %02d:%02d:%02d.%02d, ", eh, em, es, ed);
    if(blocks == 0)
        printf("ETA: N/A, ");
    else
        printf("ETA: %02d:%02d:%02d.%02d, ", rh, rm, rs, rd);

    printf("macro blocks: %lu/%lu, orbits: %lu, points: %lu\n", blocks, maxblocks, orbits, points);

    fflush(stdout);
}
