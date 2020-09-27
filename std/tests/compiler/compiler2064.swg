#test
{
    var x: [2] const *void
    var y: u64
    x[0] = cast(const *void) y
}