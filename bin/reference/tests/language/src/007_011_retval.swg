/**
# `retval` Type
Inside a function, you can use the `retval` type, which acts as an alias to the function's return type. This allows you to work with the return value within the function more conveniently.
*/

#test
{
    func toto() -> s32
    {
        var result: retval
        result = 10
        return result
    }

    @assert(toto() == 10) // The function returns 10 using the `retval` type.
}

/**
# Optimizing Return Values
The `retval` type also serves as a hint to the compiler, allowing it to directly reference the caller's storage when possible. This avoids unnecessary copying of complex return types, such as structs, tuples, or arrays.
*/

#test
{
    struct RGB { x, y, z: f64 }

    func getWhite() -> RGB
    {
        // To avoid unnecessary clearing of the returned struct, we use `= undefined`.
        var result: retval = undefined
        result.x = 0.5
        result.y = 0.1
        result.z = 1.0
        return result
    }

    // The `getWhite` function can directly modify `r`, `g`, and `b` without storing
    // a temporary value on the stack.
    let (r, g, b) = getWhite()
    @assert(r == 0.5)
    @assert(g == 0.1)
    @assert(b == 1.0)
}

/**
# Returning Arrays Efficiently
Using `retval` is the preferred method to return a struct or an array, as it is optimal in terms of performance. By avoiding unnecessary memory operations, such as clearing or copying, the function can return large or complex data structures efficiently.
*/

#test
{
    func toto() -> [255] s32
    {
        // To avoid clearing the array, we use `= undefined`.
        var result: retval = undefined
        loop i in 255:
            result[i] = i
        return result
    }

    var arr = toto()
    @assert(arr[0] == 0)    // Verifies that the first element is correctly set.
    @assert(arr[100] == 100) // Verifies that the 101st element is correctly set.
    @assert(arr[254] == 254) // Verifies that the last element is correctly set.
}
