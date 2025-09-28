#global testerror "Err0233"
struct A
{

}

impl A
{
    func opSlice(me, lower, upper: u64)->u64
    {
        return lower + upper
    }
}

#test
{
    var x: A
    var res = x[1 to 2]
    @assert(res == 3)
}