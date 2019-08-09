# pod_reflection
This is a simple C++ one-header STL based library implementing reflection for POD data types.

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