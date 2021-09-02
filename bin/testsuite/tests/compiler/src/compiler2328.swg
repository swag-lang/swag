/*var G32: s32
var G64: s64

#test
{
    var x: bool
    var y: u16
    var z: u64

    #assert(@offsetof(x) == 0)
    #assert(@offsetof(y) == 2)
    #assert(@offsetof(z) == 8)

    #assert(@offsetof(G32) & (@alignof(G32) - 1) == 0)
    #assert(@offsetof(G64) & (@alignof(G64) - 1) == 0)
}*/