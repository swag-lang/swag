#global skip // remove typeset from language the 21 july 2021
#global testerror

#test
{
    typeset XX
    {
        Bool {bool}
        Int  {s64}
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