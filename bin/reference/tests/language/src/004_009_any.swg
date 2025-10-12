/**
# The 'any' Type in Swag

'any' is a dynamically typed reference that can point to a value of any concrete type.

> WARNING:
> 'any' is **not** a variant. It holds a reference to an existing value plus its runtime type info.
*/

#test
{
    var a: any

    // Store an s32 literal value in the 'any'
    a = 6
    @assert(cast(s32) a == 6)

    // Now store a string
    a = "string"
    @assert(cast(string) a == "string")

    // Now store a bool
    a = true
    @assert(cast(bool) a == true)
}

/**
# Working with 'any' and Pointers

Use '@dataof' to access a pointer to the underlying value stored inside 'any'.
*/

#test
{
    let a:  any = 6
    let ptr = cast(const *s32) @dataof(a)
    @assert(dref ptr == 6)
}

/**
# Type Information and 'any'

'#typeof' on an 'any' yields 'any' (the reference type).
Use '@kindof' to get the concrete runtime type of the referenced value.
*/

#test
{
    var a: any = "string"
    #assert(#typeof(a) == any)
    @assert(@kindof(a) == string)

    a = true
    @assert(@kindof(a) == bool)
}

/**
# Retrieving Values from 'any'

You can retrieve the stored value directly or as a constant reference.
*/

#test
{
    let a: any = 42
    #assert(#typeof(a) == any)
    @assert(@kindof(a) == s32)

    let b = cast(s32) a // Get the value itself
    @assert(b == 42)

    let c = cast(const &s32) a // Get a constant reference to the value
    @assert(c == 42)
}

/**
# Arrays of 'any'

You can create heterogeneous arrays where each element holds a different type.
*/

#test
{
    var array: [?] any = [true, 2, 3.0, "4"]

    @assert(@kindof(array[0]) == bool)
    @assert(@kindof(array[1]) == s32)
    @assert(@kindof(array[2]) == f32)
    @assert(@kindof(array[3]) == string)

    @assert(cast(bool) array[0] == true)
    @assert(cast(s32) array[1] == 2)
    @assert(cast(f32) array[2] == 3.0)
    @assert(cast(string) array[3] == "4")
}

/**
# Nullability of 'any'

An 'any' value can be null, similar to pointers and other nullable types.
*/

#test
{
    var x: any
    @assert(x == null)

    x = 6
    @assert(x != null)
    @assert(cast(s32) x == 6)

    x = "string"
    @assert(x != null)
    @assert(cast(string) x == "string")

    x = null
    @assert(x == null)
}

/**
# Type Checking with 'any'

An 'any' can be compared to a type using '==' or '!='.
This uses '@kindof' internally to compare the stored value's type.
*/

#test
{
    var x: any
    @assert(x == null)

    x = 6
    @assert(x == s32)
    @assert(x != bool)

    x = "string"
    @assert(x != s32)
    @assert(x == string)

    struct A {}
    x = A{}
    @assert(x == A)
}
