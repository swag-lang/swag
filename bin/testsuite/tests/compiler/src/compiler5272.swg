#global testerror "Err0115"
func(v: string) toto(x: u8) {}
func(v: string) toto(x: u16) {}

#test
{
    toto'"a"(` `'u8)
    toto'"a"(0)
}