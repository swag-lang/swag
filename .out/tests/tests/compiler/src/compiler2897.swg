struct A
{

}

impl A
{
    func opIndex(using self, idx: uint) => 666
}

#test
{
    var t: A
    var r: const&A = t
    @assert(t[0] == 666)
    @assert(r[0] == 666)
    @assert(t[0] == r[0])
}