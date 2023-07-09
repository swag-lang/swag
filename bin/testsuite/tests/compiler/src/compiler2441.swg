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
            return {v + o.v}
        #if op == "*"
            return {v * o.v}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    var y: B

    {
        var res = y + x
        @assert(res.v == 15)
    }

    {
        var res = y * x
        @assert(res.v == 50)
    }
}