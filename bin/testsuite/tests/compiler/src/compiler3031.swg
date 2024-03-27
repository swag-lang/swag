#global skip // no more relative pointers (18 july 2021)
//#[Swag.PrintBc]
#test
{
    var x: string~64
    x = "toto"
    @assert(x == "toto")
}