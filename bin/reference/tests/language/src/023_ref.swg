/**
Swag has also references, which are pointers that behave like values.
*/

#test
{
    x := 42

    // Use 'ref' to declare a reference.
    // Here we declare a reference to the variable 'x'.
    var myRef: const ref s32 = &x

    // This is a pointer that behaves like a value, so no explicit dereferencing is necessary.
    // You can see this as a kind of an alias.
    @assert(myRef == 42)
}

/**
When an affectation is done outside of an initialization, you will change the pointed value,
and not the reference itself.
*/

#test
{
    x := 42
    var myRef: ref s32 = &x // Note here that the reference is no more 'const'
    @assert(myRef == 42)

    // Here we will change the pointed value 'x'
    myRef = 66
    @assert(myRef == 66)

    // Remember that 'myRef' is an alias for 'x'.
    @assert(x == 66)
}

/**
If you want to change the reference and not the pointed value, you can use 'ref'.
*/

#test
{
    x := 1
    y := 1000

    var myRef: const ref s32 = &x
    @assert(myRef == 1)

    // Here we force 'myRef' to point to 'y' and not to 'x' anymore.
    // We do *NOT* change the value of 'x'.
    myRef = ref &y
    @assert(myRef == 1000)
}