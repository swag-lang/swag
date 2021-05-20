//#[Swag.printbc]
#test
{
    enum Enum { A; B; C; }

    a := 0
    loop @typeof(Enum).values
        a += 1
    @assert(a == 3)
}