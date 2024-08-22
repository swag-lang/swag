/**
`any` is a specific type in Swag that can store values of any other type.

> WARNING:
> `any` is **not a variant**. It's a dynamically typed **reference** to an existing value. This means that `any` does not store a copy of the value, but rather a reference to the actual value, along with its type information.
*/

#test
{
    var a: any    // Declare a variable of type 'any'

    // Store an 's32' literal value in the 'any'
    a = 6

    // To access the value stored inside 'any', you need to cast it to the desired type.
    @assert(cast(s32) a == 6)

    // Now store a string instead of the 's32' value
    a = "string"
    @assert(cast(string) a == "string")

    // Now store a bool instead
    a = true
    @assert(cast(bool) a == true)
}

/**
`any` is effectively a pointer to the value it references, along with a `typeinfo` that describes the type of the value. You can use `@dataof` to retrieve the pointer to the actual value.
*/
#test
{
    let a: any = 6                         // Store an s32 value in 'a'
    let ptr = cast(const *s32) @dataof(a)  // Retrieve the pointer to the stored value
    @assert(dref ptr == 6)                 // Dereference the pointer to get the value
}

/**
When you use `@typeof` on an `any`, it will return the type `any` itself, as `any` is the type of the reference. However, `@kindof` gives you the actual underlying type of the value stored in `any`. Note that `@kindof` is evaluated at runtime.
*/
#test
{
    var a: any = "string"              // Store a string in 'a'
    #assert @typeof(a) == any          // The type of 'a' is 'any'
    @assert(@kindof(a) == string)      // The underlying type of the value is 'string'

    a = true                           // Change the stored value to a bool
    @assert(@kindof(a) == bool)        // Now the underlying type is 'bool'
}

/**
You can retrieve the value stored in an `any` either directly or as a constant reference. This flexibility allows you to work with the value in whatever way best suits your needs.
*/
#test
{
    let a: any = 42                    // Store an s32 value in 'a'
    #assert @typeof(a) == any          // The type of 'a' is 'any'
    @assert(@kindof(a) == s32)         // The underlying type is 's32'

    let b = cast(s32) a                // Get the value itself
    @assert(b == 42)
    let c = cast(const &s32) a         // Get a constant reference to the value
    @assert(c == 42)
}

/**
You can also create arrays that contain multiple types using `any`. This allows for a heterogeneous collection where each element can be of a different type. This is useful in cases where you need to store and manipulate data of various types in a single structure.
*/
#test
{
    var array: [] any = [true, 2, 3.0, "4"]  // An array containing different types
    @assert(@kindof(array[0]) == bool)       // The first element is a bool
    @assert(@kindof(array[1]) == s32)        // The second element is an s32
    @assert(@kindof(array[2]) == f32)        // The third element is an f32
    @assert(@kindof(array[3]) == string)     // The fourth element is a string

    @assert(cast(bool) array[0] == true)
    @assert(cast(s32) array[1] == 2)
    @assert(cast(f32) array[2] == 3.0)
    @assert(cast(string) array[3] == "4")
}

/**
An `any` value can be set to null and tested against null, just like pointers or other nullable types. This feature allows you to handle cases where a value may or may not be set, providing flexibility in managing optional values.
*/
#test
{
    var x: any                          // Declare an 'any' type variable
    @assert(x == null)                  // Initially, 'x' is null

    x = 6                               // Store an s32 value in 'x'
    @assert(x != null)                  // Now 'x' holds a value
    @assert(cast(s32) x == 6)

    x = "string"                        // Change the stored value to a string
    @assert(x != null)
    @assert(cast(string) x == "string")

    x = null                            // Set 'x' back to null
    @assert(x == null)
}

/**
An `any` value can be tested against a type using `==` and `!=`. This effectively calls `@kindof` to retrieve the underlying type for comparison. This capability is useful when you need to ensure that the value inside `any` is of a specific type before performing operations on it.
*/
#test
{
    var x: any                          // Declare an 'any' type variable
    @assert(x == null)                  // 'x' is null
    @assert(x != s32)                   // 'x' does not contain an s32

    x = 6                               // Store an s32 value in 'x'
    @assert(x == s32)                   // Now 'x' contains an s32
    @assert(x != bool)                  // But it does not contain a bool

    x = "string"                        // Change the stored value to a string
    @assert(x != s32)                   // Now 'x' contains a string, not an s32
    @assert(x == string)                // Confirm it holds a string

    struct A {}                         // Define a simple struct

    x = A{}                             // Store an instance of A in 'x'
    @assert(x == A)                     // 'x' is of type A
}
