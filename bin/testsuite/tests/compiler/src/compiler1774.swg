interface A
{
    func x(self);
}

struct X
{

}

impl A for X
{
    func impl x(using self)
    {

    }
}

//#[Swag.PrintBc]
#test
{
    var v: X
    let x: A = v

    @assert(x != null)
}