#global testerror "Err0297"
struct A
{

}

impl A
{
    func opSlice(using self, lower, upper: u64)->u64
    {
        return lower + upper
    }
}

#test
{
    var x: A
    var res = x[1..2]
    @assert(res == 3)
}