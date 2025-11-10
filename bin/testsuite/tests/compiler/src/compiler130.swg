#global testerror "Err0194"

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
