# pod_reflection
This is a simple C++ one-header STL based library implementing reflection for POD data types.

## Limitations:
C++ itself does not provide adequate means for reflection, so this library's
implementation is based around various template metaprogramming tools and hacks.
Though this code is written according to the standard, there are some C++ features
that make it impossible to use this library. Here is the list of known limitations:
- **Structs with default values for elements:** in C++ 11 structure is not considered as a POD in case it has default initializations 
for any of its elements. In C++ 14 a structure is allowed to have default values for elements
  and to remain a POD.
- **Structs with bitfields:** It is impossible to get an address of a bitfield member of class in C++.
Accessing a first bitfield element of an addressable location is possible,
  but is still considered UB. So it is restricted to use (no valid results will be provided)
  `eld::get` and `eld::for_each` on PODS that contain bitfields.
- **Structs with fixed size arrays**: it seems to be impossible to deduce that an element is 
a fixed size array because arrays can only be initialized only with brace initialization. 
  Thus an attempt to deduce an element will yield a type of an array element.
  **Structs with fixed size array elements are prohibited from usage with
  `eld::get`, `eld::for_each`**



## Public API:
The following public functions are implemented within the library:
*include/pod_reflection/pod_reflection.h* - new header to be used.

- `eld::pod_size<POD>` - get size of a POD structure at compile time
- `eld::pod_element_t<I, POD, TupleFeed>` - get a type of an Ith element of a POD
structure. Tuple with feed types `TupleFeed` is required to deduce the Ith element type.
- `deduced& get<I, TupleFeed>(POD& pod)` - extract the Ith element of POD using `TupleFeed`. 
This function requires testing for a particular compiler since it utilizes UB (`reinterpret_cast` from an offset).-
- `int eld::for_each<TupleFeed>(POD& pod, Callable &&callable)` - one of the most useful
functions in practice. For each element in POD object calls a callable object. The most obvious use case
  as can be seen from example - is to log (print) the contents of a POD structure.
  The most useful case which this library can be used - **automatic endian conversion** for each
  of the POD's element.
  

### Example usage:
See *example/main.cpp*.
In your CMakeLists.txt declare: `target_link_libraries(YourTarget PRIVATE eld::pod_reflection)`


### Contributing:
This library requires automated testing with CI integration. Any help is appreciated.