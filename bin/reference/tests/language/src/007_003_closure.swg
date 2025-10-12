/**
# Introduction to Closures in Swag

Swag supports limited closures, allowing functions to capture variables from
their surrounding scope. Up to 48 bytes can be captured without heap allocation.
Only simple types (without 'opDrop', 'opPostCopy', or 'opPostMove') can be captured.
*/

/**
# Declaring a Closure

A closure is declared like a lambda, with captured variables listed between '|...|'
before the parameter list. The type uses 'func||(...)'.
*/
#test
{
    let a = 125
    let b = 521

    let fct: func||() = func|a, b|()
    {
        @assert(a == 125)
        @assert(b == 521)
    }

    fct()
}

/**
# Capturing Variables by Reference

Use '&' to capture by reference; otherwise, capture is by value.
*/
#test
{
    var a = 125

    let fct: func||() = func|&a|()
    {
        a += 1
    }

    fct()
    @assert(a == 126)

    fct()
    @assert(a == 127)
}

/**
# Assigning Lambdas to Closure Variables

A closure variable can also hold a regular lambda (no captures).
*/
#test
{
    var fct: func||(s32, s32)->s32

    fct = func(x, y) => x + y
    @assert(fct(1, 2) == 3)
}

/**
# Capturing Complex Types

Arrays, structs, and slices can be captured by value if they fit in the capture size
and are plain data (POD).
*/
#test
{
    var x = [1, 2, 3]

    var fct: func||(s32)->s32

    fct = func|x|(toAdd)
    {
        var res = 0
        foreach v in x do
            res += v
        res += toAdd
        return res
    }

    let result = fct(4)
    @assert(result == 1 + 2 + 3 + 4)
}

/**
# Modifying Captured Variables

Captured values belong to the closure and can be mutated, enabling stateful behavior.
*/
#test
{
    func getInc()->func||()->s32
    {
        let x = 10

        return func|x|()->s32
        {
            x += 1
            return x
        }
    }

    let fct = getInc()

    @assert(fct() == 11)
    @assert(fct() == 12)
    @assert(fct() == 13)
}
