#global testerror "Err0376"

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
