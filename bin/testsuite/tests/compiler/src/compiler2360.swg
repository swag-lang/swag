func toto(x: string, y: s32, z: ...)
{

}

#test
{
    let str = "string"
    str.toto(10)
    str.toto(10, true)
    str.toto(10, true, 30)
}