# MPICapsule
A prototype framework implementing Apache Spark functionalities in MPI

## The framework
MPI Capsule is a protoype framework implementing the map/reduce functionalities of the Apache Spark distributed computing framework in C++ with MPI. The goal of MPI Capsule is to leverage the high performance capabilities of MPI while offering a simple programming interface that is similar to the one provided by Spark.

The name of MPI Capsule comes from the idea that the framework provides functionalities that were implemented with the complex, low-level interface of MPI, but that were encapsulated in an easy to use object-oriented interface.

## Context of development
MPI Capsule was developed in the context of my bachelor's thesis at the University of Geneva. The thesis, included in this directory (*SparkvsMPI.pdf* file) and written in French, contains an extensive performance comparison between Spark and MPI. The results of this comparison justified the development of MPI Capsule as an example of what could be done to combine the strengths of both frameworks and merge them into a single one.

## How to use MPI Capsule
MPI Capsule was implemented in the form of a header-only library. To use it in one of your projects, all you need to do is copy the contents of the */src/include* folder of this repository into your program's source and add an `#include <mpi_capsule.hpp>` statement in your code.

## Examples
Example programs using MPI Capsule are provided in the */src/examples* folder of this repository. The */data* folder contains a small text file (23 MB) that can be used in the wordcount example.

## External libraries
This project uses [Cereal](https://uscilab.github.io/cereal/), an open source data serialization library under the BSD license.
