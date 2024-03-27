#global testerror "Err0491"

#[Swag.AttrUsage(Swag.AttributeUsage.StructVariable)]
attr toto()

#[toto]
{
    #[toto]
    struct TT
    {
        b: bool
    }
}
