#global testerror "Err0238"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto()

struct TT
{
    #[toto, toto]
    b:   bool
}
