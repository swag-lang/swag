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

//#[swag.printbc]
#test
{
    x := Color.Alpha
}