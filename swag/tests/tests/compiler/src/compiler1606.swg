const ALPHA = true

enum Color
{
    Red
    Green
    Blue

    #ast
    {
        var arr: [100] u8
        arr[0] = cast(u8) "A"'char
        arr[1] = cast(u8) "A"'char
        arr[2] = 0
        return @mkstring(&arr[0], 2)
    }
}

//#[swag.printbc]
#test
{
    @assert(cast(s64) Color.AA == 3)
}