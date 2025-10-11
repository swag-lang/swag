struct A
{
    v    = 10
}

struct B
{
    v    = 5
}

impl A
{
    func(op: string) opBinary(me, o: B)->A
    {
        #if op == "+" do
            return {me.v + o.v}
        #if op == "*" do
            return {me.v * o.v}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    var y: B

    {
        let res = y + x
        @assert(res.v == 15)
    }

    {
        let res = y * x
        @assert(res.v == 50)
    }
}
