struct A
{
    v = 10
}

impl A
{
    func(op: string) opBinary(using me, o: s32)->A
    {
        #if op == "+":
            return {v + o}
        #if op == "*":
            return {v * o}
        #if op == "/":
            return {v / o}
        #if op == "%":
            return {v % o}
        #if op == "|":
            return {v | o}
        #if op == "&":
            return {v & o}
        #if op == "^":
            return {v ^ o}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A

    {
        var res = 1 + x
        @assert(res.v == 11)
    }

    {
        var res = x * 2
        @assert(res.v == 20)
    }

    {
        var res = 2 * x
        @assert(res.v == 20)
    }

    {
        var res = x / 2
        @assert(res.v == 5)
    }

    {
        var res = x % 3
        @assert(res.v == 1)
    }

    {
        var res = x | 1
        @assert(res.v == 11)
    }

    {
        var res = x & 1
        @assert(res.v == 0)
    }

    {
        var res = x ^ 1
        @assert(res.v == 11)
    }
}