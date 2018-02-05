#include <string>
#include <iostream>
#include <vector>
#include <mpi_capsule.hpp>

using namespace std;

// We define a function to convert a string of numeric values to
// an actual vector of floats.
vector<float> string2float(string& str){
  vector<float> numericValues;

  char* c_data = new char[str.size()+1];
	strcpy(c_data, str.c_str());
	c_data[str.size()] = '\0';

	char* c_value = strtok(c_data, ",\n");
	while (c_value){
		float value = stof(c_value);
		numericValues.push_back(value);
    c_value = strtok(NULL, ",\n");
	}
	delete [] c_data;

	return numericValues;
}

// We define another function that computes the sum of the squares of
// values in a vector of floats.
float sum_of_squares(vector<float>& data){
  float sum  = 0;
  for (auto f : data){
    sum += f*f;
  }
  return sum;
}


int main(int argc, char **argv){

  // An MPI_Context object must always be initialized with the argc and argv
  // arguments at the beginning of any MPICapsule program.
  MPI_Context context(argc, argv);

  try {
    // The textfile entered as first argument to the program is
    // read in parallel on the processors.
    auto dText = context.textFile(argv[1], ',');

    // The values in the textfile are read as floating point numbers
    // in a vector, then the sum of their squares is computed, and
    // finally the result is reduced on a single processor.
    auto result = dText.map(string2float).map(sum_of_squares).reduce([] (float& f1, float& f2){return f1+f2;});

    // Finally, the content of the ReducedData object orderedData is printed out using a lambda function as print function.
    result.printData([](float& f) {cout << "Final result : " << f << endl;});

  }
  // When calling 'MPI_Context::textFile()', a try catch block must always be used
	// to handle the case where the file entered as parameter couldn't be opened.
  catch (exception const& e){
    if (context.getRank() == 0)
      cerr << e.what() << endl;
  }

  // The 'finalize' method of the MPI_Context object must always
  // be called at the end of a MPI_Capsule program.
  context.finalize();

  return 0;
}
