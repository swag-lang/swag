#global testerror "Err0019"
#[Swag.Overload]
func(v: string) toto(x: u8)
{
}
#[Swag.Overload]
func(v: string) toto(x: u16)
{
}

#test
{
    toto'"a"(` `'u8)
    toto'"a"(0)
}
