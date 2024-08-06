#global testerror "Err0742"
func toto(x: u32){}

#test
{
    toto(4294967295)
}