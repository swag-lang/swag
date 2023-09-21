//#global testerror "Err0182"

struct A0
{
    x: s32 = 666
}

impl A0
{
    #[Swag.Macro]
    mtd opIndex(idx: u64)->s32
    {
        return 0
    }
}

struct A1
{
    x: s32 = 666
}

impl A1
{
    #[Swag.Macro]
    mtd opIndex(idx: u64)->s32
    {
        return 0
    }
}

struct B
{
    using a: A0
    using b: A1
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v: B
    var x = v[0]
}
