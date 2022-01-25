//func toto(x: s32) {}

#test
{
    var v: closure(s32)
    @assert(@sizeof(v) == 8 * @sizeof(*void))
    //v = &toto
}