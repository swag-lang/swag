/**
# The `retval` special type
In Swag, the `retval` type acts as an alias to the function's return type. This feature allows developers to handle 
the return value within the function in a more convenient and flexible manner. By using `retval`, you can define 
and manipulate the variable intended to be returned, without explicitly specifying its type. This abstraction 
simplifies code maintenance and enhances readability, especially when dealing with complex return types.
*/

#test
{
    func toto() -> s32
    {
        var result: retval                         // 'retval' is equivalent to 's32' in this context.
        result = 10                                // Assign a value to 'result'.
        return result                              // Return the value stored in 'result'.
    }

    @assert(toto() == 10)                          // The function returns 10 using the `retval` type.
}

/**
# Optimizing return values
The `retval` type also serves as an optimization hint to the compiler. When used correctly, it allows the compiler 
to reference the caller's storage directly, bypassing unnecessary copies of the return value. This is particularly 
beneficial for functions returning large or complex types, such as structs, tuples, or arrays, where performance 
can be significantly improved by reducing memory operations.
*/

#test
{
    struct RGB { x, y, z: f64 }

    func getWhite() -> RGB
    {
        // Using `retval = undefined` avoids unnecessary clearing of the returned struct.
        var result: retval = undefined             // 'retval' here is equivalent to 'RGB'.
        result.x = 0.5                             // Assign value to the 'x' field.
        result.y = 0.1                             // Assign value to the 'y' field.
        result.z = 1.0                             // Assign value to the 'z' field.
        return result                              // Return the fully initialized struct.
    }

    // The `getWhite` function allows direct assignment to the tuple (r, g, b) without additional storage.
    let (r, g, b) = getWhite()
    @assert(r == 0.5)                              // Verifies the 'x' field is correctly assigned.
    @assert(g == 0.1)                              // Verifies the 'y' field is correctly assigned.
    @assert(b == 1.0)                              // Verifies the 'z' field is correctly assigned.
}

/**
# Returning arrays efficiently
The use of `retval` is highly recommended when returning large data structures like arrays or structs. 
By leveraging `retval`, you can avoid unnecessary memory operations, such as clearing or copying large objects, 
resulting in more efficient and performant code. This approach is especially useful in performance-critical 
applications where minimizing overhead is crucial.
*/

#test
{
    func toto() -> [255] s32
    {
        // Using `retval = undefined` avoids clearing the array, improving performance.
        var result: retval = undefined             // 'retval' here is an array of 255 integers.
        loop i in 255:                             // Loop through each index in the array.
            result[i] = i                          // Assign the index value to each array element.
        return result                              // Return the fully populated array.
    }

    var arr = toto()
    @assert(arr[0] == 0)                           // Verifies that the first element is correctly set.
    @assert(arr[100] == 100)                       // Verifies that the 101st element is correctly set.
    @assert(arr[254] == 254)                       // Verifies that the last element is correctly set.
}
