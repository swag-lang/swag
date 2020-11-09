struct A
{

}

impl A
{
    func opSlice(using self, lower, upper: u32)->u32
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