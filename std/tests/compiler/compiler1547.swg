using swag

#[constexpr]
func hasMember(type: const* TypeInfo, name: string)->bool
{
    if(name == "titi")
        return true
    return false
}

struct X
{

}

impl X
{
    func titi()
    {

    }
}

//#[swag.printbc]
#test
{
    #if hasMember(@typeof(X), "toto")
    {
        @assert(false)
    }

    x := false
    #if hasMember(@typeof(X), "titi")
    {
        x = true
    }    

    @assert(x == true)
}