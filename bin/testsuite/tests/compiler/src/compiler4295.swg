#global testerror "Err0583"
func toto(x: u32){}

#test
{
    toto(4294967295)
}