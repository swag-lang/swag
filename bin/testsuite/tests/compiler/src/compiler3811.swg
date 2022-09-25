//#global testerror
struct A
{
    x: RR
}

struct B
{
    a: A
}

enum RR { T; }

impl A
{
    mtd toto(rr: RR)
    {
        x = rr
    }
}

#test
{
    var b: B
    with b.a
    {
        .x = .T
        .toto(.T)
        .toto(.T)
        .toto(.T)
    }
}