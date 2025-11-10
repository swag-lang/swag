var c: s32 = 0

//#[Swag.PrintBc]
#test
{
    let op = "toto1"
    if (op == "toto") do
        c = 1
    elif (op == "toto1") do
        c = 2
    @assert(c == 2)
}
