// Swag supports a limited set of closure
// Only a given amount of bytes of capture are possible (for now 48 bytes)
// That way there's never an hidden allocation
// You can also only capture 'simple' variables (no struct with 'opDrop', 'opPostCopy', 'opPostMove' for example)

#test
{
    // Captured are declared like lambdas, with the captured variables between |...| and before
    // parameters
    {
        a := 125
        b := 521

        var fct: closure() = @|a, b|()
        {
            @assert(a == 125)
            @assert(b == 521)
        }

        fct()
    }

    // You can capture by pointer
    {
        a := 125
        var fct: closure() = @|&a|()
        {
            dref a += 1
        }

        fct()
        @assert(a == 126)
        fct()
        @assert(a == 127)
    }

    // You can assign a normal lambda (no capture) to a closure variable
    {
        var fct: closure(s32, s32)->s32

        fct = @(x, y) => x + y
        @assert(fct(1, 2) == 3)
    }

    // You can capture arrays, structs, slices etc... as long as it fits in the
    // maximum storage of 'n' bytes
    {
        x := @[1, 2, 3]

        var fct: closure(s32)->s32
        fct = @|x|(toAdd)
        {
            res := 0
            visit v: x
                res += v
            res += toAdd
            return res
        }

        result := fct(4)
        @assert(result == 1 + 2 + 3 + 4)
    }

    // Captured variables are copied, and are mutable
    {
        func getInc()->closure()->s32
        {
            x := 10
            return @|x|()->s32 {
                x += 1
                return x
            }
        }

        fct := getInc()
        @assert(fct() == 11)
        @assert(fct() == 12)
        @assert(fct() == 13)
    }
}