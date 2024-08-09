#global testerror "Err0622"
func toto(x: u32){}

#test
{
    toto(4294967295)
}