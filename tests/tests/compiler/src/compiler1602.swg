struct A
{
    #ast "x: s32"
    #ast "y: s32 = 666"
}

#test
{
    var a: A
    a.x = 0
    a.y = 666
}