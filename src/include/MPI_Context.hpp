#ifndef __MPI_CONTEXT_H__
#define __MPI_CONTEXT_H__

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "mpi.h"
#include "DistributedData.hpp"
#include "MPI_SendRecv.hpp"
#include "FileError.hpp"

class MPI_Context
{
	private :

	int nProc;
	int rank;
	int master;

	public :

	/**
	 * \brief This constuctor initializes MPI at the beginning of a MPICapsule program :
				any program using MPICapsule must instantiate a MPI_Context object at its
				beginning.
	 * \param argc The 'argc' argument of the 'main' function of the program using MPICapsule
					must always be used for this parameter.
	 * \param argv The 'argv' argument of the 'main' function of the program using MPICapsule
					must always be used for this parameter.
	*/
	MPI_Context(int argc, char **argv) : master(0){
		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &nProc);
	}

	/**
	 * \brief This constuctor initializes MPI at the beginning of a MPICapsule program :
				any program using MPICapsule must instantiate a MPI_Context object at its
				beginning.
	 * \param argc The 'argc' argument of the 'main' function of the program using MPICapsule
				must always be used for this parameter.
	 * \param argv The 'argv' argument of the 'main' function of the program using MPICapsule
				must always be used for this parameter.
	 * \param masterRank This parameters allows the user to determine the rank of the master
				node in the program. By default, its rank is 0 (when the other
				constructor is called).
	*/
	MPI_Context(int argc, char **argv, int masterRank) : master(masterRank) {
		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &nProc);
	}

	int getNProc() const {
		return nProc;
	}

	int getRank() const {
		return rank;
	}

	int getMaster() const {
		return master;
	}

	void setMaster(int masterRank){
		master = masterRank;
	}

	/**
	* \brief This method opens a file in parallel on all the processors of the program and loads in a
				string on each one of them a chunk of the file, for posterior treatment in parallel
				(distribution of the file).
 	* \param filename The name (path) of the file to distribute.
	* \param delimiter A character indicating the delimiter that can be used to separate the file in partitions.
 	* \return A DistributedData<std::string> object on each node of the program containing a partition of
				the file that needed to be distributed.
	*/
	DistributedData<std::string> textFile(char* filename, char delimiter) {
		// The file is opened on all processors in parallel.
		MPI_File textfile;
		int fileOpened = MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &textfile);

		if (fileOpened != MPI_SUCCESS){
			// If the file entered as parameter can't be opened, an error is
			// thrown.
			throw FileError();
		}

		// Each worker retrieves the size of the file opened.
		MPI_Offset filesize;
		MPI_File_get_size(textfile, &filesize);

		// Each processor computes the size of the part of the file
		// it will locally process.
	 	MPI_Offset localsize = filesize/nProc;
		// Processors determine the starting and ending points of their part in the file.
		MPI_Offset start = rank*localsize;
		MPI_Offset end = start+localsize-1;

		// The last processor finishes at the end of the file.
		if (rank == nProc-1)
			end = filesize;
		// localsize is recomputed to take into account the potential difference of size for the
		// last processor's chunk.
		localsize = end-start+1;

		// Memory allocation of a C string.
		char *chunk = (char*)malloc((localsize+1)*sizeof(char));
		// Processors all read their respective chunk of the file.
		MPI_File_read_at_all(textfile, start, chunk, localsize, MPI_CHAR, MPI_STATUS_IGNORE);
		chunk[localsize] = '\0';
		MPI_File_close(&textfile);

		// The chunk of the file to be locally processed by each processor is copied in a
		// local std::string.
		std::string localString = chunk;
		free(chunk);

		// The first 'word' (first string of characters until the next delimiter) is sent by each processor
		// of rank higher than 0 to its 'left neighbour' and will then be ignored in the localString.
		if (rank > 0){
			std::string first;
			std::size_t delim_pos = localString.find(delimiter)+1;
			first = localString.substr(0, delim_pos);
			MPI_SendRecv::send(first, rank-1, 0, MPI_COMM_WORLD);

			// The string read with localString.substr() is not included in the localString
			// anymore.
			localString.erase(0, delim_pos);
		}

		// The string received from the right neighbour by each proc. is added at the end
		// of localString (except for the last processor, of rank nProc-1).
		if (rank < nProc-1){
			std::string last;
			MPI_SendRecv::recv(last, rank+1, 0, MPI_COMM_WORLD);
			localString.append(last);
		}

		DistributedData<std::string> data(getRank(), getNProc(), getMaster(), localString);
		return data;
	}

	/**
	 * \brief This method executes MPI_Finalize() and must always
			 be called at the end of any program using MPI_Capsule.
	*/
	void finalize() {
		MPI_Finalize();
	}
};

#endif
