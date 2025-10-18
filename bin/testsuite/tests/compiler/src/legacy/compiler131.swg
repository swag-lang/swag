#global testerror "Err0197"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto()

struct TT
{
    #[toto, toto]
    b:   bool
}
