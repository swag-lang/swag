#global testerror "Err0656"
func toto(x: u32){}

#test
{
    toto(4294967295)
}