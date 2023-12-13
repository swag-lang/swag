struct V {}
impl V
{
    mtd(op: string) const opBinary(other: const &V)->Self
    {
        return V{}
    }
}

//#[Swag.PrintBc]
func toto(a, b: const &V)
{
    var c = a - b
    #assert @typeof(c) == V
}
