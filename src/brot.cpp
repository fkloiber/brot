#include <cstdio>

#include <mpi.h>

#include <brot/options.h>
#include <brot/rand.h>
#include <brot/iterate.h>

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

    MPI_Finalize();
    return 0;
}
