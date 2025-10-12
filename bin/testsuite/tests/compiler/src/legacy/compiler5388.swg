interface I
{
    mtd const x();
}

struct(T) A
{
    buffer: [*] T
}

var G: I

impl A
{
    #[Swag.Inline]
    mtd opIndex(x: u64)->const &T
    {
        return &G
    }
}

#[Swag.Safety(.All, .All, false)]
func toto()
{
    var arr: A'I
    arr[0].x()
}
