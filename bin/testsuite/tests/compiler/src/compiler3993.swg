struct V {}
impl V
{
    mtdc(op: string) opBinary(other: const ref V)->Self
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
