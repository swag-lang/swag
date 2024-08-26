#global testerror "Err0598"
func toto(x: u32){}

#test
{
    toto(4294967295)
}