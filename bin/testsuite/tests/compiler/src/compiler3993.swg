struct V {}
impl V
{
    mtd(op: string) const opBinary(other: const ref V)->Self
    {
        return V{}
    }
}

//#[Swag.PrintBc]
func toto(a, b: const ref V)
{
    c := a - b
    #assert @typeof(c) == V
}
