using Swag

#[ConstExpr]
func hasMember(type: const *TypeInfo, name: string)->bool
{
    if (name == "titi") do
        return true
    return false
}

struct X {}

impl X
{
    func titi() {}
}

//#[Swag.PrintBc]
#test
{
    #if hasMember(#typeof(X), "toto")
    {
        @assert(false)
    }

    var x = false
    #if hasMember(#typeof(X), "titi")
    {
        x = true
    }

    @assert(x == true)
}
