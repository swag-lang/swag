#global testerror "Err0563"
func toto(x, y: s32, z: bool)
{

}

#test
{
    toto(z: true, 1, 2)
}