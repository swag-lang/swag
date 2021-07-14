#global testerror

#test
{
    typeset XX
    {
        Bool {bool}
        Int  {int}
    }

    var x: XX = Bool{true}

    #[Swag.Complete]
    switch x
    {
    case Bool:
        break
    case Int:
        break
    default:
        break;
    }
}