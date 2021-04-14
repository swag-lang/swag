#test
{
    {
        // By default, swag will panic if some operators overflow.

        x := 255'u8
        //x += 1      // This would overflow, and panic, because we lose informations
        //x <<= 1     // Same here, we lose some bits
    }

    {
        // Operators that can overflow are :
        // + - * << >> and their equivalent += -= *= <<= >>=
        //
        // If you know what your are doing, you can use a special version of those operators, which will not panic.
        // Add ',safe' after the operation. This will disable safety checks.

        x := 255'u8
        x +=,safe 1     // wrap around
        @assert(x == 0)

        y := 255'u8
        y >>=,safe 1
        @assert(y == 127)
    }

    {
        // Note that you can remove safety checks with a special swag attribute
        // Can be defined for a file, a function, or just a piece of code.
        // "of" stands for "OverFlow".
        #[swag.safety("of", false)]
        {
            x := 255'u8
            x += 255    // 254
            x += 1      // 255
            x >>= 1     // 127
            @assert(x == 127)
        }
    }
}