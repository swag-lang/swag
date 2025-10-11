//#global skip
#[Swag.Macro]
func Visit(str: const [..] u8)
{
    let ptr   = @dataof(str)
    let count = @countof(str)
    @assert(ptr[0] == 97)
    @assert(count == 3)
}

//#[Swag.PrintBc]
#test
{
    let str = "abc"
    foreach str
    {
    }
}
