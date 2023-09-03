#global testerror "Err0053"
func toto(x: u32){}

#test
{
    toto(4294967295)
}