#global skip
struct AA
{
    s = "toto"
}

//#[Swag.PrintBc]
#test
{
    var arr: [4] AA
    var ref = cast(const &AA) arr[0]
    var ref1 = cast(const &AA) ref
    @assert(ref.s == "toto")
    @assert(ref1.s == "toto")

    /*foreach v in arr
    {

    }*/
}