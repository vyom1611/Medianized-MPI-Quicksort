#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <sys/time.h>


/**
* @brief Output the seconds elapsed while sorting. This excludes input and
*        output time. This should be wallclock time, not CPU time.
*
* @param seconds Seconds spent sorting.
*/
static void print_time(
    double const seconds)
{
  printf("Sort Time: %0.04fs\n", seconds);
}

int random_int() {
    return rand();
}

int compare(const void * a, const void * b) {
    return (*(int*)a - *(int*)b);
}

int quickselect(int* arr, int n) {
    qsort(arr, n, sizeof(int), compare);
    return arr[n/2];
}

int partition(int* arr, int n, int pivot) {
    int i = 0;
    int j = n - 1;
    while (1) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i >= j) return j;
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
        i++;
    }
}

int get_overall_median(int local_median, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int* medians = NULL;
    if (rank == 0) {
        medians = malloc(size * sizeof(int));
    }
    MPI_Gather(&local_median, 1, MPI_INT, medians, 1, MPI_INT, 0, comm);

    int overall_median = local_median; 
    if (rank == 0) {
        qsort(medians, size, sizeof(int), compare);
        overall_median = medians[size / 2];
        free(medians);
    }
    MPI_Bcast(&overall_median, 1, MPI_INT, 0, comm);
    return overall_median;
}


void parallel_quicksort(int* arr, int n, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (n <= 1) return;

    int local_median = quickselect(arr, n);
    int overall_median = get_overall_median(local_median, comm);

    int partition_idx = partition(arr, n, overall_median);

    // Split the communicator into two groups
    MPI_Comm new_comm;
    int color = (rank < size/2) ? 0 : 1;
    MPI_Comm_split(comm, color, rank, &new_comm);

    if (color == 0) {
        parallel_quicksort(arr, partition_idx + 1, new_comm);
    } else {
        parallel_quicksort(arr + partition_idx + 1, n - partition_idx - 1, new_comm);
    }

    MPI_Comm_free(&new_comm);
}

int main(int argc, char** argv) {
    int rank, size, N, segmentSize;
    int* numbers;
    struct timeval start_time, end_time; 

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0)
            printf("Usage: mpirun -np <num_procs> %s <N> <output_file>\n", argv[0]);
        MPI_Finalize();
        return 0;
    }

    N = atoi(argv[1]);
    segmentSize = N / size;

    numbers = malloc(segmentSize * sizeof(int));

    // Generate numbers
    for (int i = 0; i < segmentSize; i++) {
        numbers[i] = random_int(rank);
    }

    // Capture starting time
    if (rank == 0) {
        gettimeofday(&start_time, NULL);
    }

    // Parallel quicksort
    parallel_quicksort(numbers, segmentSize, MPI_COMM_WORLD);

    // Capture ending time and compute elapsed time
    if (rank == 0) {
        gettimeofday(&end_time, NULL);
        double elapsed_seconds = (end_time.tv_sec - start_time.tv_sec) + 
                                (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        print_time(elapsed_seconds); 
    }

    // Gather sorted segments at rank 0
    int* sorted = NULL;
    if (rank == 0) {
        sorted = malloc(N * sizeof(int));
    }
    MPI_Gather(numbers, segmentSize, MPI_INT, sorted, segmentSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Rank 0 writes to file
    if (rank == 0) {
        FILE* file = fopen(argv[2], "w");
        fprintf(file, "%d\n", N);
        for (int i = 0; i < N; i++) {
            fprintf(file, "%d\n", sorted[i]);
        }
        fclose(file);
        free(sorted);
    }

    free(numbers);
    MPI_Finalize();

    return 0;
}

