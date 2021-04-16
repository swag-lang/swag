#global testerror

#test
{
    typeset XX
    {
        Bool {bool}
        Int  {int}
    }

    var x: XX = Bool{true}

    #[swag.complete]
    switch x
    {
    case Bool:
        break
    }
}