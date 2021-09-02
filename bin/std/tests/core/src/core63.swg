using Core.Reflection

#[Swag.ExportType("methods")]
struct X
{

}

impl X
{
    func toto()
    {

    }
}

//#[Swag.PrintBc]
#test
{
    tt := @typeof(X)
    @assert(hasMethod(tt, "toto"))
    @assert(!hasMethod(tt, "toto1"))
}