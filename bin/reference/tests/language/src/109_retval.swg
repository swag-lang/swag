#test
{
    // Inside a function, you can use the 'retval' type which is an alias
    // to the function return type
    {
        func toto()->s32
        {
            var result: retval
            result = 10
            return result
        }

        @assert(toto() == 10)
    }

    // But 'retval' will also make a direct reference to the caller storage, to
    // avoid an unnecessary copy (if possible). So this is mostly a hint for the
    // compiler, and usefull when the function returns a complexe type like a
    // struct or an array.
    {
        struct RGB { x, y, z: f64; }

        func getWhite()->RGB
        {
            // To avoid the clear of the struct, we use = ?
            var result: retval = ?
            result.x = 0.5
            result.y = 0.1
            result.z = 1.0
            return result
        }

        // Here the 'getWhite' function can directly modify r, g and b without storing
        // a temporary value on the stack.
        (r, g, b) := getWhite()
        @assert(r == 0.5)
        @assert(g == 0.1)
        @assert(b == 1.0)
    }

    // This is the preferred way (because optimal) to return a struct or an array.
    {
        func toto()->[255] s32
        {
            // To avoid the clear of the array, we use = ?
            var result: retval = ?
            loop i: 255
                result[i] = i
            return result
        }

        arr := toto()
        @assert(arr[0] == 0)
        @assert(arr[100] == 100)
        @assert(arr[254] == 254)
    }
}