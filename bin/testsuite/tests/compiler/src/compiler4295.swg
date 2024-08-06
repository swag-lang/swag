#global testerror "Err0609"
func toto(x: u32){}

#test
{
    toto(4294967295)
}