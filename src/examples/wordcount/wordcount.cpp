#include <string>
#include <iostream>
#include <unordered_map>
#include <map>
#include <mpi_capsule.hpp>

using namespace std;

/* We define a function that counts the words in an std::string object
and saves them and their number of occurrences in a std::unordered_map. */
unordered_map<string,int> count_words(string& str){
	unordered_map<string,int> wordcount;

	char* c_text = new char[str.size()+1];
	strcpy(c_text, str.c_str());
	c_text[str.size()] = '\0';

	char* c_word = strtok(c_text, " \n");
	while (c_word){
		std::string word(c_word);
		if (!word.empty())
			wordcount[word]++;
		c_word = strtok(NULL, " \n");
	}
	delete [] c_text;

	return wordcount;
}

/* We define a function that takes two unordered_map objects as input and merges them into a single one. */
unordered_map<string,int> merge(unordered_map<string,int>& mapA, unordered_map<string,int>& mapB){
	unordered_map<string,int> newMap(mapA);

	for (auto it=mapB.begin(); it!=mapB.end(); ++it)
		newMap[(*it).first] += (*it).second;

	return newMap;
}

/* We define a function to sort the elements of an unordered_map and return them in a map. */
map<string,int> sortMap(unordered_map<string,int>& unord_map){
	map<string,int> ordered(unord_map.begin(), unord_map.end());
	return ordered;
}


int main(int argc, char **argv){

	// An MPI_Context object must always be initialized with the argc and argv
	// arguments at the beginning of any MPICapsule program.
	MPI_Context context(argc, argv);

	try {
		double t1 = MPI_Wtime();

		// The text in the file entered as first argument to the program is
		// read in parallel on the processors.
		auto dText = context.textFile(argv[1], ' ');

		double t2 = MPI_Wtime();

		// The words are counted locally by each processor on its part of the file,
		// and the result of the counting is then reduced to a single processor.
		auto counts = dText.map(count_words).reduce(merge);

		double t3 = MPI_Wtime();

		if (context.getRank() == 0){
			cout << "Time to readthe file : " << t2-t1 << endl;
			cout << "Time to count the words : " << t3-t2 << endl;
		}

		// Only processor 0 (the master node) has the result of the wordcount in its
		// ReducedData object.
		// We transform the content of the ReducedData object 'counts' to a map so
		// its keys are ordered.
		auto orderedData = counts.map(sortMap);
		// Finally, the content of the ReducedData object orderedData is printed out using a lambda function as print function.
		orderedData.printData([](map<string,int>& map) { for (auto kv : map) cout << kv.first << " : " << kv.second << endl; });

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
