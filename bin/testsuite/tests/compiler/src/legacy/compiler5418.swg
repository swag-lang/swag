struct A {}

impl A
{
    mtd opIndex(x: u64)->&string
    {
        return &g
    }
}

var g = "titi"

func toto(x: const [..] u8)
{
    @assert(@countof(x) == 4)
}

//#[Swag.PrintBc]
#test
{
    var a: A
    toto(a[0])
}
