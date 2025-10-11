struct A { r, g, b, d: s32 }

var G = 0
impl A
{
    mtd opPostCopy()
    {
        G += 1
    }
}

#[Swag.Strict]
alias AL = A

#test
{
    var a: [4] AL
    var b: [4] AL
    a = b
    @assert(G == 4)
}
