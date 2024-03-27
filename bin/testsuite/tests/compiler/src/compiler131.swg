#global testerror "Err0068"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto()

struct TT
{
    #[toto, toto]
    b: bool
}
