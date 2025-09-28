struct V {}
impl V
{
    mtd(op: string) const opBinary(other: const &V)->V
    {
        return V{}
    }
}

//#[Swag.PrintBc]
func toto(a, b: const &V)
{
    let c = a - b
    #assert(#typeof(c) == V)
}
