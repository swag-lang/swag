func toto()->s32 => 0

#test
{
    var t: [2] func()->s32
    t[0] = &toto
}
