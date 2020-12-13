#test
{
    var x: bool
    var y: u16
    var z: u64

    #assert(@offsetof(x) == 0)
    #assert(@offsetof(y) == 2)
    #assert(@offsetof(z) == 8)
}