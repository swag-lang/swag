#[Swag.Overload]
{
    func toto();
    func toto();
    func toto();
}

var G = 0
#[Swag.Overload]
func toto()
{
    G = 1
}

#test
{
    toto()
    @assert(G == 1)
}
