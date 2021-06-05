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

//#[Swag.printbc]
#test
{
    x := Color.Alpha
}