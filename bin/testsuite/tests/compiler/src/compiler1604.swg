const ALPHA = true

enum Color
{
    Red
    Green
    Blue

    #if ALPHA
    {
        Alpha
    }
}

//#[Swag.PrintBc]
#test
{
    var x = Color.Alpha
}