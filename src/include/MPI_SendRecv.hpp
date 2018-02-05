#ifndef __MPI_SendRecv_H__
#define __MPI_SendRecv_H__

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "mpi.h"
#include "Serialization.hpp"

class MPI_SendRecv
{
	public :
	
	/**
	 \brief This method sends an array of data of type T and of length 'len'
			to another processor on the 'comm' communicator of MPI.  
	 \param data The adress of the data to be sent.
	 \param len The length of the data array to be sent.
	 \param dest The rank of the destination node for the message.
	 \param tag MPI tag of the message.
	 \param comm MPI communicator on which the message must be sent.
	*/
	template<typename T>
	static void send(T const& data, int len, int dest, int tag, MPI_Comm comm);
	
	/**
	 \brief This method allows a processor to receive an array of data of type T
			and of length 'len' from another processor on the 'comm' communicator.
	 \param data The adress of the array in which the data must be retrieved.
	 \param len The length of the array to be received.
	 \param src The rank of the source node of the message.
	 \param tag MPI tag of the message.
	 \param comm MPI communicator on which the message is received.
	*/
	template<typename T>
	static void recv(T& data, int len, int src, int tag, MPI_Comm comm);
	
	
	/**
	 \brief This method sends an STL container of data of type T 
			to another processor on the 'comm' communicator of MPI.  
	 \param data The adress of the data to be sent.
	 \param dest The rank of the destination node for the message.
	 \param tag MPI tag of the message.
	 \param comm MPI communicator on which the message must be sent.
	*/
	template<typename T>
	static void send(T const& data, int dest, int tag, MPI_Comm comm);
	
	/**
	 \brief This method allows a processor to receive an STL container of data 
			of type T from another processor on the 'comm' communicator.
	 \param data The adress of the container in which the data must be retrieved.
	 \param src The rank of the source node of the message.
	 \param tag MPI tag of the message.
	 \param comm MPI communicator on which the message is received.
	*/
	template<typename T>
	static void recv(T& data, int src, int tag, MPI_Comm comm);

};

/* Send and receive methods for arrays of basic datatypes. */

/* char */
template<>
void MPI_SendRecv::send(char const& data, int len, int dest, int tag, MPI_Comm comm){
	MPI_Send(&data, len, MPI_CHAR, dest, tag, comm);
}

template<>
void MPI_SendRecv::recv(char& data, int len, int src, int tag, MPI_Comm comm){
	MPI_Status s;
	MPI_Recv(&data, len, MPI_CHAR, src, tag, comm, &s);
}

/* int */
template<>
void MPI_SendRecv::send(int const& data, int len, int dest, int tag, MPI_Comm comm){
	MPI_Send(&data, len, MPI_INT, dest, tag, comm);
}

template<>
void MPI_SendRecv::recv(int& data, int len, int src, int tag, MPI_Comm comm){
	MPI_Status s;
	MPI_Recv(&data, len, MPI_INT, src, tag, comm, &s);
}

/* float */
template<>
void MPI_SendRecv::send(float const& data, int len, int dest, int tag, MPI_Comm comm){
	MPI_Send(&data, len, MPI_FLOAT, dest, tag, comm);
}

template<>
void MPI_SendRecv::recv(float& data, int len, int src, int tag, MPI_Comm comm){
	MPI_Status s;
	MPI_Recv(&data, len, MPI_FLOAT, src, tag, comm, &s);
}

/* double */
template<>
void MPI_SendRecv::send(double const& data, int len, int dest, int tag, MPI_Comm comm){
	MPI_Send(&data, len, MPI_DOUBLE, dest, tag, comm);
}

template<>
void MPI_SendRecv::recv(double& data, int len, int src, int tag, MPI_Comm comm){
	MPI_Status s;
	MPI_Recv(&data, len, MPI_DOUBLE, src, tag, comm, &s);
}

/* short */
template<>
void MPI_SendRecv::send(short const& data, int len, int dest, int tag, MPI_Comm comm){
	MPI_Send(&data, len, MPI_LONG, dest, tag, comm);
}

template<>
void MPI_SendRecv::recv(short& data, int len, int src, int tag, MPI_Comm comm){
	MPI_Status s;
	MPI_Recv(&data, len, MPI_LONG, src, tag, comm, &s);
}

/* long */
template<>
void MPI_SendRecv::send(long const& data, int len, int dest, int tag, MPI_Comm comm){
	MPI_Send(&data, len, MPI_LONG, dest, tag, comm);
}

template<>
void MPI_SendRecv::recv(long& data, int len, int src, int tag, MPI_Comm comm){
	MPI_Status s;
	MPI_Recv(&data, len, MPI_LONG, src, tag, comm, &s);
}

/* Send and receive methods for std::string objects. */
template<>
void MPI_SendRecv::send(std::string const& str, int dest, int tag, MPI_Comm comm){
	unsigned len = str.size();
	MPI_Send(&len, 1, MPI_UNSIGNED, dest, tag, comm);
	if (len != 0)
		MPI_Send(str.data(), len, MPI_CHAR, dest, tag, comm);
}

template<>
void MPI_SendRecv::recv(std::string& str, int src, int tag, MPI_Comm comm){
	unsigned len;
	MPI_Status s;
	MPI_Recv(&len, 1, MPI_UNSIGNED, src, tag, comm, &s);

	if (len != 0){
		std::vector<char> tmp(len);
		MPI_Recv(tmp.data(), len, MPI_CHAR, src, tag, comm, &s);
		str.assign(tmp.begin(), tmp.end());
	}
	else
		str.clear();
}

/* Send and receive methods for all std containers (vectors or maps for example). */
template<typename T>
void MPI_SendRecv::send(T const& data, int dest, int tag, MPI_Comm comm){
	std::string serializedData = Serialization<T>::serialize(data);
	send(serializedData, dest, tag, comm);
}

template<typename T>
void MPI_SendRecv::recv(T& data, int src, int tag, MPI_Comm comm){
	std::string recvStr;
	recv(recvStr, src, tag, comm);
	data = Serialization<T>::deserialize(recvStr);
}

#endif