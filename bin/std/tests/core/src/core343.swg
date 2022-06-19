using Core

#test
{
    var arr: Array'u8
    loop i: 100
        arr.add(cast(u8) i)

    var comp: Compress.Deflate
    try comp.init(arr.toSlice())
    res := try comp.compress()
}