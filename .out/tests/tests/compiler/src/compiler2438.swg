struct A
{
    v = 10
}

impl A
{
    func(op: string) opBinary(using self, o: s32)->A
    {
        #if op == "+"
            return @{v + o}
        #if op == "*"
            return @{v * o}
        #if op == "/"
            return @{v / o}
        #if op == "%"
            return @{v % o}
        #if op == "|"
            return @{v | o}
        #if op == "&"
            return @{v & o}
        #if op == "^"
            return @{v ^ o}
    }
}

//#[swag.printbc]
#test
{
    var x: A

    {
        res := 1 + x
        @assert(res.v == 11)
    }

    {
        res := x * 2
        @assert(res.v == 20)
    }

    {
        res := 2 * x
        @assert(res.v == 20)
    }

    {
        res := x / 2
        @assert(res.v == 5)
    }

    {
        res := x % 3
        @assert(res.v == 1)
    }

    {
        res := x | 1
        @assert(res.v == 11)
    }

    {
        res := x & 1
        @assert(res.v == 0)
    }

    {
        res := x ^ 1
        @assert(res.v == 11)
    }
}