/**
# References in Swag

Swag also supports **references**, which are pointers that behave like values. References in Swag provide
a convenient way to work with memory addresses while abstracting away the need for explicit pointer
syntax, making them easier and safer to use in many cases.
*/

#test
{
    var x = 42

    // Use '&' to declare a reference.
    // Here, we declare a reference to the variable 'x'.
    // Unlike C++, you must take the address of 'x' to convert it into a reference.
    let myRef: const &s32 = &x // Declare a constant reference to 'x'

    // References behave like values, so no explicit dereferencing is needed.
    // You can think of this as an alias for 'x'.
    @assert(myRef == 42) // Verify the reference holds the value of 'x'
}

/**
# Assigning to References

When an assignment is made to a reference outside of its initialization, the operation changes the value
of the variable being referenced, not the reference itself.
*/

#test
{
    var x      = 42
    var myRef: &s32 = &x     // The reference is mutable because it's not declared 'const'
    @assert(myRef == 42) // Confirm the reference initially points to 'x' (value 42)

    // This changes the value of 'x' through the reference.
    myRef = 66 // Update the value through the reference
    @assert(myRef == 66) // Ensure the reference now holds the updated value

    // Since 'myRef' is an alias for 'x', 'x' is also updated.
    @assert(x == 66) // Confirm the original variable 'x' has been updated as well
}

/**
# Reassigning References

Unlike in C++, you can change the reference itself (reassign it) rather than the value it points to. To
reassign the reference, use the `ref` modifier in the assignment.
*/

#test
{
    var x = 1
    var y = 1000

    var myRef: &s32 = &x // Initialize reference to point to 'x'
    @assert(myRef == 1) // Verify the reference points to 'x' (value 1)

    // Here, we reassign 'myRef' to point to 'y' instead of 'x'.
    // The value of 'x' remains unchanged.
    myRef = #ref &y // Reassign reference to point to 'y'
    @assert(myRef == 1000) // Ensure the reference now points to 'y' (value 1000)
}

/**
# Passing References to Functions

Most of the time, you need to take the address of a variable to create a reference to it. The only
exception is when passing a reference to a function parameter, and the reference is declared as `const`.
In such cases, taking the address explicitly is not necessary.
*/

#test
{
    // We can pass a literal directly because 'x' in the function 'toto' is a const reference.
    // No need to take the address manually.
    toto(4) // Pass a literal to the function 'toto'
}

func toto(x: const &s32)
{
    @assert(x == 4) // Validate the received reference matches the passed literal

    // Internally, the reference is still an address to an 's32'.
    let ptr = &x // Obtain the address from the reference
    @assert(dref ptr == 4) // Dereference the pointer to validate the value
}

/**
# Using References with Structs

This approach is particularly useful for structs, as it allows passing literals directly to functions.
*/

// Our first simple struct!
struct MyStruct { x: s32, y: s32 }

#test
{
    titi0({1, 2}) // Test passing a literal tuple to 'titi0'
    titi1({3, 4}) // Test passing a literal tuple to 'titi1'
    titi2({5, 6}) // Test passing a struct to 'titi2'
}

func titi0(param: const &{ x: s32, y: s32 })
{
    // We'll discuss tuples and field naming later, but for now, we can access tuple items by position.
    @assert(param.item0 == 1) // Validate first tuple item
    @assert(param.item1 == 2) // Validate second tuple item
}

/**
# Equivalent Reference Passing

Note that declaring a tuple type or a struct type as a parameter is equivalent to passing a constant
reference.
*/

func titi1(param: { x: s32, y: s32 })
{
    @assert(param.x == 3) // Access struct fields by name
    @assert(param.y == 4) // Validate field values
}

func titi2(param: MyStruct)
{
    @assert(param.x == 5) // Access fields of 'MyStruct'
    @assert(param.y == 6) // Validate field values
}
