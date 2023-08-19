/**
Swag supports a limited set of the `closure` concept.

Only a given amount of bytes of capture are possible (for now 48 bytes). That way there's never an hidden allocation.
Another limitation is that you can only capture 'simple' variables (no struct with `opDrop`, `opPostCopy`, `opPostMove` for example).
*/

/**
A closure is declared like a lambda, with the captured variables between `|...|` before the function parameters.
*/
#test
{
    let a = 125
    let b = 521

    // Capture 'a' and 'b'
    let fct: closure() = closure|a, b|()
    {
        @assert(a == 125)
        @assert(b == 521)
    }

    fct()
}

/**
You can also capture by pointer with `&` (otherwise it's a copy).
*/
#test
{
    var a = 125

    // Capture 'a' by pointer
    let fct: closure() = closure|&a|()
    {
        // We can change the value of the local variable 'a'
        dref a += 1
    }

    fct()
    @assert(a == 126)
    fct()
    @assert(a == 127)
}

/**
You can also capture by reference with `ref`.
*/
#test
{
    var a = 125

    // Capture 'a' by reference
    let fct = closure|ref a|()
    {
        // We can change the value of the local variable 'a'
        a += 1
    }

    fct()
    @assert(a == 126)
    fct()
    @assert(a == 127)
}

/**
You can assign a normal lambda (no capture) to a closure variable.
*/
#test
{
    var fct: closure(s32, s32)->s32

    fct = func(x, y) => x + y
    @assert(fct(1, 2) == 3)
}

/**
You can capture arrays, structs, slices etc... as long as it fits in the maximum storage of 'n' bytes (and as long as the struct is a pod).
*/
#test
{
    var x = [1, 2, 3]

    var fct: closure(s32)->s32

    // Capture the array 'x' by making a copy
    fct = closure|x|(toAdd)
    {
        var res = 0
        visit v: x
            res += v
        res += toAdd
        return res
    }

    let result = fct(4)
    @assert(result == 1 + 2 + 3 + 4)
}

/**
Captured variables are mutable, and part of the closure. So you can modify them.
*/
#test
{
    func getInc()->closure()->s32
    {
        let x = 10

        return closure|x|()->s32 {
            x += 1
            return x
        }
    }

    let fct = getInc()
    @assert(fct() == 11)
    @assert(fct() == 12)
    @assert(fct() == 13)
}
