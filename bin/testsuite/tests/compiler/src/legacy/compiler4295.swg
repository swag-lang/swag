#global testerror "Err0604"
func toto(x: u32) {}

#test
{
    toto(4294967295)
}
