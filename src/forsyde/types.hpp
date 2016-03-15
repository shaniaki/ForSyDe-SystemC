/**********************************************************************           
    * TypeContainer::get().hpp -- provides a simple type reflection mechanism.       *
    *                                                                 *
    * Author:  Hosein Attarzadeh (shan2@kth.se) based on:             *
    * http://stackoverflow.com/questions/1055452/c-get-name-of-type-in-template *
    *                                                                 *
    * Purpose: Provide facilities to store the type names, used in    *
    *          introspection.                                         *
    * Usage:   This file is included automatically                    *
    *                                                                 *
    * License:                                                        *
    *******************************************************************/

#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
 #include <sstream>
#include "rapidxml_print.hpp"


using namespace rapidxml;

/*! \file TypeContainer::get().hpp
 * \brief Provides facilities for basic type introspection
 * 
 *  This file includes a a set of basic facilities for registering names
 * for non-POD C/C++ TypeContainer::get() to be reflected in the XML output of the
 * interospection stage.
 */

//~ namespace ForSyDe
//~ {


// The general case uses RTTI (if the type is not registered explicitly)
#pragma once
template<typename T> const char* get_type_name() {
	return typeid(T).name();
}


template <typename T>
using Vector = std::vector<T>;

template <typename... T>
using Tuple = std::tuple<T...>;

// Specialization for each type
#define DEFINE_TYPE(X) \
    template<>const char* get_type_name<X>(){return #X;}
// Another version where we explicitly provide the type name (for complex TypeContainer::get())
#define DEFINE_TYPE_NAME(X,N) \
    template<>const char* get_type_name<X>(){return N;}

// Specialization for base TypeContainer::get()

DEFINE_TYPE(char);
DEFINE_TYPE(short int);
DEFINE_TYPE(unsigned short int);
DEFINE_TYPE(int);
DEFINE_TYPE(unsigned int);
DEFINE_TYPE(long int);
DEFINE_TYPE(unsigned long int);
DEFINE_TYPE(long long int);
DEFINE_TYPE(unsigned long long int);
DEFINE_TYPE(bool);
DEFINE_TYPE(float);
DEFINE_TYPE(double);
DEFINE_TYPE(long double);
DEFINE_TYPE(wchar_t);


static char* const_name = (char*)"name";
static char* const_data_type = (char*)"data_type";
static char* const_primitive = (char*)"primitive";
static char* const_vector = (char*)"vector";
static char* const_tuple = (char*)"tuple";
static char* const_size = (char*)"size";
static char* const_length = (char*)"length";
static char* const_root_type = (char*)"forsyde_types";

class TypeContainer {
public:
	static TypeContainer& get() {
		static TypeContainer instance;
		return instance;
	}

	xml_node<>* root() { return root_node; }
	xml_document<>* doc() { return &xml_doc; }


    xml_node<>* add_node(xml_node<>* parent, const char* name) {
        xml_node<>* node = xml_doc.allocate_node(node_element, name);
        parent->append_node(node);
        return node;
    }

    void add_attribute(xml_node<>* node, const char* attr_name, const char* attr_val) {
        xml_attribute<>* attr = xml_doc.allocate_attribute(attr_name, attr_val);
        node->append_attribute(attr);
    }

    //! The print method writes the XML file to the output.
    /*! The XML structure is already generated, so this command only
     * checks for the availability of the output file and dumps the XML
     * to it.
     */
    void printXML(std::string fileName)
    {
        std::ofstream outFile(fileName);
        if (!outFile.is_open())
            SC_REPORT_ERROR(fileName.c_str(), "file could not be opened to write the introspection output. Does the path exists?");
        outFile << "<?xml version=\"1.0\" ?>" << std::endl;
        outFile << "<!-- Automatically generated by ForSyDe -->" << std::endl;
        //outFile << "<!DOCTYPE process_network SYSTEM \"forsyde.dtd\" >"  << std::endl;
        outFile << xml_doc;

    }

private:
	TypeContainer() {
		root_node = xml_doc.allocate_node(node_element, const_root_type);
		xml_doc.append_node(root_node);
	};

	TypeContainer(TypeContainer const&);
	void operator=(TypeContainer const&);

	xml_document<> xml_doc;
	xml_node<>* root_node;

};

class IntrospectiveType {
public:
	template <typename T>
	static inline const char* traverse() {
		xml_node<>* root = TypeContainer::get().root();
		const char* type_name = get_type_name<T>();

		for (auto child = root->first_node(); child; child = child->next_sibling()) {
			if (child->first_attribute(const_name)->value() == type_name ) return type_name;
		}

		xml_node<> *type_node = TypeContainer::get().add_node(root, const_data_type);
		TypeContainer::get().add_attribute(type_node, const_name, type_name);
		add_type_node<T>::get(type_node);
		return type_name;
	};

	template <typename T>
	struct add_type_node {
		static inline void get (xml_node<>* parent) {
			xml_node<> *primitive_node = TypeContainer::get().add_node(parent, const_primitive);
			TypeContainer::get().add_attribute(primitive_node, const_name, get_type_name<T>());
			TypeContainer::get().add_attribute(primitive_node, const_size, size_to_char(sizeof(T)));
		}
	};

	template <typename T>
	struct add_type_node<Vector<T>> {
		static inline void  get (xml_node<>* parent) {
			xml_node<> *vector_node = TypeContainer::get().add_node(parent, const_vector);

			add_type_node<T>::get(vector_node);

			//size_t child_size = char_to_size(vector_node->first_node()->first_attribute(const_size)->value());
			//TypeContainer::get().add_attribute(vector_node, const_length, size_to_char(sizeof(T)/child_size));
			TypeContainer::get().add_attribute(vector_node, const_size, size_to_char(sizeof(Vector<T>)));
		}
	};

	template <typename... T>
	struct add_type_node<Tuple<T...>> {
		static inline void get (xml_node<>* parent) {
			xml_node<> *tuple_node = TypeContainer::get().add_node(parent, const_tuple);
			traverse_tuple<sizeof...(T), Tuple<T...>>::get(tuple_node);
			//TypeContainer::get().add_attribute(tuple_node, const_size, size_to_char(sizeof(Tuple<T...>)));
		}
	};

	template <size_t N, typename Tup>
	struct traverse_tuple {};

	template <size_t N, typename Head, typename... Tail>
	struct traverse_tuple<N, Tuple<Head, Tail...>>{
		static inline void get (xml_node<>* parent) {
			add_type_node<Head>::get(parent);
			traverse_tuple<N-1, Tuple<Tail...>>::get(parent);
		}
	};

	template <typename Head, typename... Tail>
	struct traverse_tuple<1, Tuple<Head, Tail...>> {
		static inline void get (xml_node<>* parent) {
			add_type_node<Head>::get(parent);
		}
	};

    static inline const char* size_to_char(size_t size){
    	std::string str = std::to_string(size);
    	char * writable = new char[str.size() + 1];
    	std::copy(str.begin(), str.end(), writable);
    	writable[str.size()] = '\0';
    	return writable;
    }

    static inline size_t char_to_size(const char* char_size){
    	std::istringstream iss(char_size);
    	size_t size;
    	iss >> size;
    	return size;
    }
};

//~ }

#endif
