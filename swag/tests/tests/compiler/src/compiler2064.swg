#test
{
    var x: [2] const *void
    var y: uint
    x[0] = cast(const *void) y
}