#global testerror

#test
{
    typeset XX
    {
        Bool {bool}
        Int  {int}
    }

    var x: XX = Bool{true}

    #[Swag.complete]
    switch x
    {
    case Bool:
        break
    }
}