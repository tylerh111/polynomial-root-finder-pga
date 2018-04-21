//
// Created by tdh5188 on 4/12/18.
//

#include "Worker.h"
#include "../Local.h"


Worker::Worker(int pid, std::string pname, Polynomial& polynomial, Population& population)
        : polynomial(polynomial), population(population) {
    this->pid = pid;
    this->pname = std::move(pname);
    for(int& i : this->buffer) i = 0;
}



/*MPI_Send(
        void* data,
        int count,
        MPI_Datatype datatype,
        int destination,
        int tag,
        MPI_Comm communicator)
MPI_Recv(
        void* data,
        int count,
        MPI_Datatype datatype,
        int source,
        int tag,
        MPI_Comm communicator,
        MPI_Status* status)*/
int Worker::mainProcedure() {

    //initialize population
    population.init();

    /*std::cout << "\n\n\n";
    std::cout << "pop\t|\tcomb" << std::endl;
    for (int i = 0; i < population._population_size; i++){
        std::cout << population._population[i] << "\t|\t" << population._combined_population[i] << std::endl;
    }*/


    int x = -1;

    //evolutionary loop
    for (long gen = 0; gen < 100 /*|| population reaches goal*/; gen++){
        //std::cout << "starting loop" << std::endl;

        //wait to start
        MPI_Recv(&x, MAX_SIZE, MPI_INTEGER, MASTER_PID, TAG_CONTINUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //std::cout << "evolving" << std::endl;

        //evolve population once
        population.evolve();

        //std::cout << "checking" << std::endl;

        if (population.checkSolution()){
            std::cout << "we done bois" << std::endl;
            x = Population::FOUND;
        }

        MPI_Send(&x, 1, MPI_INTEGER, MASTER_PID, TAG_STATUS_UPDATE, MPI_COMM_WORLD);

        //std::cout << "getting summary" << std::endl;

        //get a summary of the population statistics
        double summary[Population::SUM_SIZE];
        population.getSummary(summary);

        //send array to master node
        MPI_Send(summary, Population::SUM_SIZE, MPI_DOUBLE, MASTER_PID, TAG_SUMMARY, MPI_COMM_WORLD);




        //TODO: log progress

        //TODO: integration / migrate
    }

    const std::complex<double> best = population.getBestFit().getChromosome();

    MPI_Send(&best, 1, MPI_DOUBLE_COMPLEX, MASTER_PID, TAG_FINAL, MPI_COMM_WORLD);

    return 0;
}
