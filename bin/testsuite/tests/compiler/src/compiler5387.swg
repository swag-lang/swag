struct(T) A
{
    buffer: ^T
}

impl A
{
    #[Swag.Inline]
    mtd opIndex(x: u64)->const &T
    {
        return buffer + x
    }
}

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    var str: [2] string
    var a: A'string
    a.buffer = &str

    str[0] = "toto"

    let b = a[0]
    @assert(b == "toto")
}