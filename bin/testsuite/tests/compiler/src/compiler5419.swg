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
    @assert(@countof(a[0]) == 4)

    let d = cast(const *u8) @dataof(a[0])
    @assert(dref d == 't')
}
