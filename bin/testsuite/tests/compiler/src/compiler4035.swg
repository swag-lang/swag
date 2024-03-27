//#global testerror

enum OT
{
    A = 1
    B
}

struct A
{
    x: typeinfo
}

func toto()
{
    var v: A{OT}
}

//#[Swag.PrintBc]
#test
{
    var vv: const *Swag.TypeInfoEnum = OT
    var pt  = {x: OT}
}
