#test
{
    typeset XX
    {
        Bool {bool}
        Int  {int}
    }

    var x: XX = Bool{true}
    switch x
    {
    case Bool:
        @print("true")
        break
    case Int:
        break
    }
}