struct A
{

}

impl A
{
    func opSlice(using self, lower, upper: uint)->uint
    {
        return lower + upper
    }
}

#test
{
    var x: A
    res := x[1..2]
    @assert(res == 3)
}