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
        return cast(string) arr
    }
}

//#[swag.printbc]
#test
{
    @assert(cast(s64) Color.AA == 3)
}