interface A
{
    x: func(self)->void
}

struct X
{

}

impl A for X
{
    func x(using self)
    {

    }
}

//#[swag.printbc]
#test
{
    var v: X
    var x: A = v

    @assert(x != null)
}