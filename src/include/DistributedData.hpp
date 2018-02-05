#ifndef __DISTRIBUTEDDATA_H__
#define __DISTRIBUTEDDATA_H__

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "mpi.h"
#include "MPI_SendRecv.hpp"
#include "ReducedData.hpp"

template <typename T>
class DistributedData
{
	private :
	
	int procRank;
	int nProcs;
	int masterProc;
	T data;
	
	public :
	
	DistributedData(int rank, int procs, int master) : procRank(rank), nProcs(procs), masterProc(master){}
	DistributedData(int rank, int procs, int master, T localData) : procRank(rank), nProcs(procs), masterProc(master), data(localData){}
	
	T getData(){
		return data;
	}
	
	void setData(T newData){
		data = newData;
	}
	
	/**
	 \brief This method applies the function entered as parameter on the data 
			contained by the object in its 'data' attribute.
	 \param func A pointer to a function or a lambda function taking data of type T as input, 
				and returning data of any type R.
	 \return A new DistributedData object of type R, containing the result of the application
			of 'func' on the data in 'data'.
	*/
	template<typename R>
	DistributedData<R> map(R (*func)(T&)){
		R result = func(data);
		DistributedData<R> resultData(procRank, nProcs, masterProc, result);
		return resultData;
	}
	
	/**
	 \brief This method reduces the data distributed in a set of DistributedData objects
			on all processors in the MPI_WORLD_COMM communicator. It sends the data to a single 
			processor, the master of the program, and it applies the function defined in 'func'
			on the data during its reduction.
	 \param func A pointer to a function or lambda function taking two objects of type T as input
				and returning only one of the same type.
	 \return A ReducedData<T> object on each processor in the program. Only the ReducedData object
			on the master node will actually contain the result of the reduction, the rest will be empty.
	*/
	ReducedData<T> reduce(T (*func)(T&,T&)){
		// At the beginning of the reduction, all processors are active, and half 
		// of them receives from the other half their data. 
		int activeProcs(nProcs);
		int receivers((nProcs+1)/2);
		int nb_recv_odd(nProcs%2);
		
		T tmpData(data);
		
		while (activeProcs>1){
			// A processor is a sender when its rank is higher than half the number of
			// active processors and smaller than the number of active processors. 
			// After a processor has sent its data to a receiver, it becomes 'inactive'.
			if (procRank >= receivers && procRank < activeProcs){
				MPI_SendRecv::send(tmpData, procRank-receivers, 0, MPI_COMM_WORLD);
			}
			// A processor is a receiver as long as its rank is smaller than half the number
			// of active processors. Receivers apply 'func' on their data and the received one, 
			// and then send the result when they become senders.
			else if (procRank < receivers){
				if (nb_recv_odd==0 || (nb_recv_odd!=0 && procRank < receivers-1)){
					T recvData; // Container for the data received during the reduction.
					MPI_SendRecv::recv(recvData, procRank+receivers, 0, MPI_COMM_WORLD);
					tmpData = func(tmpData, recvData);
				}
			}
		
			// At every iteration, half the processor have sent their data and become
			// inactive. The variable 'nb_recv_odd' is used to determine whether there
			// will be an odd number of receivers at the next iteration. If so, the last of
			// the receivers won't receive anything during the next step.
			activeProcs = (activeProcs+1)/2;
			receivers = (activeProcs+1)/2;
			nb_recv_odd = activeProcs%2;
		}
		
		// Only the master node of the program has data in the ReducedData object it 
		// returns. All the other nodes return empty ReducedData objects.
		ReducedData<T> result(procRank, masterProc);
		if (procRank==0)
			result.setData(tmpData);
		
		return result;
	}
};

#endif