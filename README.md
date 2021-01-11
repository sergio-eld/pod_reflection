# pod_reflection
This is a simple C++ one-header STL based library implementing reflection for POD data types.
The Library can be used witt C++11, C++14 and above. However, POD structs specification differs 
between 11 and 14 standards: POD structure can not have default initialized members in C++11.


## Public API:
The following public functions are implemented within the library:
*include/pod_reflection/pod_reflection.h* - new header to be used.

- `eld::pod_size<POD>` - get size of a POD structure at compile time
- `eld::pod_element_t<I, POD, TupleFeed>` - get a type of an Ith element of a POD
structure. Tuple with feed types `TupleFeed` is required to deduce the Ith element type.
- `deduced& get<I, TupleFeed>(POD& pod)` - extract the Ith element of POD using `TupleFeed`. 
This function requires testing for a particular compiler since it utilizes UB (`reinterpret_cast` from an offset).

### Example usage:
**TODO: fill**
You 

### Contributing:
This library requires automated testing with CI integration. Any help is appreciated.

___
# OLD VERSION
*Old version is no longer supported, use new API*

Initially inspired by the question at https://stackoverflow.com/questions/45820379/get-the-number-of-fields-in-a-class/57157650#57157650

Implementation is based on using designated initializers:
- implicit conversion of the "ubiq_constructor" to deduce the maximum number of possible arguments for initialization
(see https://en.cppreference.com/w/cpp/language/aggregate_initialization), proposed by Antony Polukhin 
(https://www.youtube.com/watch?v=abdeAew3gmQ&feature=youtu.be video CppCon 2016)

- explicit conversion of "ubiq_explicit" object to deduce particular argument's type using 
provided "pod_map".

"pod_map" can be extended by user via defining a macro "POD_EXTENDS" as an std::tuple of 
user-defined types before including "pod_reflection.hpp". By default pod_map contains only
basic types (bool, 
	char, unsigned char, 
    short, unsigned short, 
    int, unsigned int, 
    long, unsigned long, 
    long long, unsigned long long, 
    float, double, long double, long double).
	
Usage: see example/main.cpp