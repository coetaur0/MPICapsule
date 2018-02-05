#ifndef __REDUCEDDATA_H__
#define __REDUCEDDATA_H__

#include <iostream>
#include <string>
#include <functional>
#include "MPI_Context.hpp"

template <typename T>
class ReducedData
{
	private:

	int procRank;
	int masterProc; 
	T data;
	
	public:
		
	ReducedData(int rank, int master) : procRank(rank), masterProc(master){}
	ReducedData(int rank, int master, T localData) : procRank(rank), masterProc(master), data(localData){}
	
	T getData(){
		return data;
	}
	
	void setData(T newData){
		data = newData;
	}
	
	/**
	 \brief This method applies a function 'func' on the data of the ReducedData<T> object calling it.
	 		It returns a new ReducedData object of type R, the return type of 'func'., containing the result
			of the function.
	 \param func A pointer to a function (or lambda function).
	 \return The result of the application of 'func' on the data in the object calling 'map', encapsulated
			in a new ReducedData object.
	*/
	template<typename R>
	ReducedData<R> map(R (*func)(T&)){
		R result = func(data);
		ReducedData<R> resultData(procRank, masterProc, result);
		return resultData;
	}
	
	/**
	 \brief This method calls the printing function 'printFunc' specified by the user on the data
			in the object calling it. Only the master processor will execute 'printFunc', since it
			is the only one actually having data in its ReducedData object. 
	 \param printFunc A function to print the data in the ReducedData object on the master node 
			(or do something else with it).
	*/
	void printData(void (*printFunc)(T&)){
		if (procRank == masterProc)
			printFunc(data);
	}
	
	/**
	 \brief This method calls the function 'saveFunc' on the data in the object calling it, in
			order to save it in a file specified by the 'filename' parameter. Only the master node 
			will actually execute 'saveFunc', since it is the only one having data in its ReducedData
			object.
	 \param saveFunc A function that saves the data of the object in a file. The function must take a 
			filename as second parameter (in a std::string const& object).
	*/
	int saveDataTo(int (*saveFunc)(T&, std::string const&), std::string const& filename){
		if (procRank == masterProc){
			int success = saveFunc(data, filename);
			return success;
		}
		else
			return 0;
	}

};

#endif