/**
# The `retval` Special Type
In Swag, `retval` represents the current function’s return type.
It allows you to declare and manipulate the return value directly inside the function,
without repeating the type declaration. This improves code readability and flexibility,
especially when working with complex or generic return types.
*/
#test
{
    func toto()->s32
    {
        var result: retval // 'retval' resolves to 's32' in this context.
        result = 10
        return result
    }

    @assert(toto() == 10)
}

/**
# Optimizing Return Values
`retval` provides an optimization hint that allows the compiler to use the caller’s memory
for the return value, reducing unnecessary copies.
This is especially beneficial when returning large structs, arrays, or tuples.
*/
#test
{
    struct RGB { x, y, z: f64 }

    func getWhite()->RGB
    {
        // 'retval = undefined' prevents unnecessary clearing of the return structure.
        var result: retval = undefined
        result.x = 0.5
        result.y = 0.1
        result.z = 1.0
        return result
    }

    let (r, g, b) = getWhite()
    @assert(r == 0.5)
    @assert(g == 0.1)
    @assert(b == 1.0)
}

/**
# Returning Arrays Efficiently
When returning large data structures like arrays, using `retval` avoids redundant
initialization or copying, resulting in faster, more memory-efficient code.
*/
#test
{
    func toto()->[255] s32
    {
        var result: retval = undefined
        for i in 255 do
            result[i] = i
        return result
    }

    var arr = toto()
    @assert(arr[0] == 0)
    @assert(arr[100] == 100)
    @assert(arr[254] == 254)
}
