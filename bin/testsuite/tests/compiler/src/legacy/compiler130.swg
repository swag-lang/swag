#global testerror "Err0287"

#[Swag.AttrUsage(Swag.AttributeUsage.StructVariable)]
attr toto()

#[toto]
{
    #[toto]
    struct TT
    {
        b:   bool
    }
}
