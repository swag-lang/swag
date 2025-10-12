/**
# References in Swag

Swag supports **references**, which behave like pointers but act syntactically like values.
References offer a safer and more convenient way to work with memory addresses, eliminating the need for explicit pointer dereferencing in most cases.
*/
#test
{
    var x = 42

    // Declare a constant reference to 'x'.
    // Unlike C++, you must explicitly take the address of 'x' to create the reference.
    let myRef: const &s32 = &x

    // References behave like aliases — no explicit dereferencing is needed.
    @assert(myRef == 42)
}

/**
# Assigning Through References

Assigning a value to a reference (after initialization) updates the **referenced variable**, not the reference itself.
*/
#test
{
    var x      = 42
    var myRef: &s32 = &x
    @assert(myRef == 42)

    // Change the value of 'x' through the reference
    myRef = 66
    @assert(myRef == 66)
    @assert(x == 66) // 'x' is updated as well
}

/**
# Reassigning References

Unlike in C++, Swag allows **reassigning** a reference to point to a different variable.
Use the `#ref` modifier to perform this operation safely.
*/
#test
{
    var x = 1
    var y = 1000

    var myRef: &s32 = &x
    @assert(myRef == 1)

    // Reassign the reference to point to 'y' instead of 'x'
    myRef = #ref &y
    @assert(myRef == 1000)
}

/**
# Passing References to Functions

Normally, you take the address of a variable to create a reference.
However, when passing a **const reference** to a function, Swag automatically takes the address — even for literals.
*/
#test
{
    // Passing a literal directly is allowed for const references
    toto(4)
}

func toto(x: const &s32)
{
    @assert(x == 4)

    // A reference is still an address internally
    let ptr = &x
    @assert(dref ptr == 4)
}

/**
# Using References with Structs

References are especially useful when working with structs.
They allow you to pass entire structures or tuples directly, including literal expressions.
*/

// Our first simple struct
struct MyStruct { x: s32, y: s32 }

#test
{
    titi0({1, 2})
    titi1({3, 4})
    titi2({5, 6})
}

func titi0(param: const &{ x: s32, y: s32 })
{
    // Tuples can be accessed by item index
    @assert(param.item0 == 1)
    @assert(param.item1 == 2)
}

/**
# Equivalent Reference Passing

Declaring a tuple type or a struct type as a function parameter is **equivalent**
to passing a **constant reference**.
*/
func titi1(param: { x: s32, y: s32 })
{
    @assert(param.x == 3)
    @assert(param.y == 4)
}

func titi2(param: MyStruct)
{
    @assert(param.x == 5)
    @assert(param.y == 6)
}
