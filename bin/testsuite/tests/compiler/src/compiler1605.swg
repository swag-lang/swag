const ALPHA = true

enum Color
{
    Red
    Green
    Blue

    #ast
    {
        return "Alpha = 10"
    }
}

//#[Swag.PrintBc]
#test
{
    var x = Color.Alpha
    @assert(cast(s64) Color.Alpha == 10)
}
