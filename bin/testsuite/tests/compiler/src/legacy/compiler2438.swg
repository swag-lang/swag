struct A
{
    v    = 10
}

impl A
{
    func(op: string) opBinary(me, o: s32)->A
    {
        #if op == "+" do
            return {me.v + o}
        #if op == "*" do
            return {me.v * o}
        #if op == "/" do
            return {me.v / o}
        #if op == "%" do
            return {me.v % o}
        #if op == "|" do
            return {me.v | o}
        #if op == "&" do
            return {me.v & o}
        #if op == "^" do
            return {me.v ^ o}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A

    {
        let res = 1 + x
        @assert(res.v == 11)
    }

    {
        let res = x * 2
        @assert(res.v == 20)
    }

    {
        let res = 2 * x
        @assert(res.v == 20)
    }

    {
        let res = x / 2
        @assert(res.v == 5)
    }

    {
        let res = x % 3
        @assert(res.v == 1)
    }

    {
        let res = x | 1
        @assert(res.v == 11)
    }

    {
        let res = x & 1
        @assert(res.v == 0)
    }

    {
        let res = x ^ 1
        @assert(res.v == 11)
    }
}
