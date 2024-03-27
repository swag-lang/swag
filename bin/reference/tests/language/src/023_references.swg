/**
Swag has also **references**, which are pointers that behave like values.
*/

#test
{
    var x = 42

    // Use '&' to declare a reference.
    // Here we declare a reference to the variable 'x'.
    // Note that unlike C++, you have to take the address of 'x' to convert it to a reference.
    let myRef: const &s32 = &x

    // This is a pointer that behaves like a value, so no explicit dereferencing is necessary.
    // You can see this as a kind of an alias.
    @assert(myRef == 42)
}

/**
When an affectation is done outside of an initialization, you will change the pointed value, and not the reference itself.
*/

#test
{
    var x      = 42
    var myRef: &s32 = &x     // Note here that the reference is no more 'const'
    @assert(myRef == 42)

    // Here we will change the pointed value 'x'
    myRef = 66
    @assert(myRef == 66)

    // Remember that 'myRef' is an alias for 'x', so 'x' has also been changed.
    @assert(x == 66)
}

/**
But unlike C++, you can change the reference (reassign it) and not the pointed value if you want.
You must then use `ref` in the affectation.
*/

#test
{
    var x = 1
    var y = 1000

    var myRef: const &s32 = &x
    @assert(myRef == 1)

    // Here we force 'myRef' to point to 'y' and not to 'x' anymore.
    // We do *NOT* change the value of 'x'.
    myRef = const ref &y
    @assert(myRef == 1000)
}

/**
Most of the time, you have to take the address of a variable to make a reference to it. The only exception are function parameters, if the reference is `const`. In that case, taking the address is not necessary
*/

#test
{
    // We can pass a literal because the parameter 'x' of 'toto' is 'const ref' and not just 'ref'.
    toto(4)
}

func toto(x: const &s32)
{
    @assert(x == 4)

    // Under the hood, you will get a const address to an 's32'
    let ptr = &x
    @assert(dref ptr == 4)
}

/**
This is usefull for structs for examples, as you can directly pass a literal to a function.
*/

struct MyStruct
{
    x:     s32
    y:     s32
}
// Our first little struct !

#test
{
    titi0({1, 2})
    titi1({3, 4})
    titi2({5, 6})
}

func titi0(param: const &{x: s32, y: s32})
{
    // We'll see later about tuples and naming of fields.
    @assert(param.item0 == 1)
    @assert(param.item1 == 2)
}

/**
Note that declaring a tuple type or a struct type is equivalent to a constant reference.
*/
func titi1(param: {x: s32, y: s32})
{
    @assert(param.x == 3)
    @assert(param.y == 4)
}

func titi2(param: MyStruct)
{
    @assert(param.x == 5)
    @assert(param.y == 6)
}
