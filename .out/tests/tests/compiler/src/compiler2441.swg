struct A
{
    v = 10
}

struct B
{
    v = 5
}

impl A
{
    func(op: string) opBinary(using self, o: B)->A
    {
        #if op == "+"
            return @{v + o.v}
        #if op == "*"
            return @{v * o.v}
    }
}

//#[swag.printbc]
#test
{
    var x: A
    var y: B

    {
        res := y + x
        @assert(res.v == 15)
    }

    {
        res := y * x
        @assert(res.v == 50)
    }
}