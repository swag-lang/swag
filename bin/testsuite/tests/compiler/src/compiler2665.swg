func toto()->{x: s32, y: s32}
{
    return {10, 20}
}

#test
{
    var h = toto()
    var x = {h}
}