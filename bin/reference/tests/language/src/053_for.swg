/**
`for` loops in Swag follow the traditional structure seen in C/C++: a *start statement*, a *test expression*, and an *ending statement*. This structure provides a versatile way to iterate over a range of values with fine control over the loop's behavior.
*/
#test
{
    var cpt = 0
    // Standard 'for' loop with initialization, condition, and increment.
    for var i = 0; i < 10; i += 1;:
        cpt += 1
    @assert(cpt == 10)

    // Semicolons ';' can be replaced by newlines for cleaner syntax.
    for var i = 0
        i < 10
        i += 1
    {
        cpt += 1
    }
    @assert(cpt == 20)

    // Instead of a single expression, you can use a statement block for the initialization or increment.
    for { var i = 0; cpt = 0; }
        i < 10
        i += 1
    {
        cpt += 1
    }
    @assert(cpt == 10)

    for { var i = 0; cpt = 0; }
        i < 10
        { i += 2; i -= 1; }
    {
        cpt += 1
    }
    @assert(cpt == 10)
}

/**
Like `loop`, `visit`, and `while`, the `for` loop gives you access to `#index`, which represents the **current loop index**. This can be useful when you need to keep track of the iteration count separately from the loop variable.
*/
#test
{
    var cpt = 0'u64
    // Use of `#index` to accumulate the loop indices.
    for var i: u32 = 10; i < 15; i += 1;:
        cpt += #index
    @assert(cpt == 0+1+2+3+4)

    var cpt1 = 0'u64
    for var i = 10; i < 15; i += 1;:
        cpt1 += #index
    @assert(cpt1 == 0+1+2+3+4)
}
