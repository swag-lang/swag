#global testerror "Err0582"
func toto(x: u32){}

#test
{
    toto(4294967295)
}