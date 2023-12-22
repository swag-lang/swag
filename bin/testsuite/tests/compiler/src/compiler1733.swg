//#[Swag.PrintBc]
#test
{
    enum Enum { A; B; C; }

    var a = 0
    loop @typeof(Enum).values do
        a += 1
    @assert(a == 3)
}