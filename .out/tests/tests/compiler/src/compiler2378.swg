#global skip
struct AA
{
    s = "toto"
}

//#[swag.printbc]
#test
{
    var arr: [4] AA
    var ref = cast(const &AA) arr[0]
    var ref1 = cast(const &AA) ref
    @assert(ref.s == "toto")
    @assert(ref1.s == "toto")

    /*visit v: arr
    {

    }*/
}