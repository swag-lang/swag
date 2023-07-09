func toto(x: string, y: s32, z: ...)
{

}

#test
{
    var str = "string"
    str.toto(10)
    str.toto(10, true)
    str.toto(10, true, 30)
}