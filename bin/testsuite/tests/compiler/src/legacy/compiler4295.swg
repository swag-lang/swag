#global testerror "Err0745"
func toto(x: u32) {}

#test
{
    toto(4294967295)
}
