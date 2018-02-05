#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__

#include <map>
#include <unordered_map>
#include <string>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/binary.hpp>

template<typename Container>
class Serialization
{	
	public :
	
	/**
	 \brief This method serializes the container it received as parameter
			and returns it in the form of an std::string object.
	 \param container An STL container to be serialized.
	 \return An std::string object containing the serialized container.
	*/
	static std::string serialize(Container const& container){
		std::stringstream ss;
	
		cereal::BinaryOutputArchive oarchive(ss);
		oarchive(container);
	
		return ss.str();
	} 

	/**
	 \brief This method deserializes the content of an std::string representing 
			a serialized container. 
	 \param serializedContainer An std::string object which contains a serialized container.
	 \return The deserialized container of type 'Container' that was in the string.
	*/
	static Container deserialize(std::string const& serializedContainer){
		std::stringstream ss(serializedContainer);
		Container container;
	
		cereal::BinaryInputArchive iarchive(ss);
		iarchive(container);
	
		return container;
	}
};


#endif