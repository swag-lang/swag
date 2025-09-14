using Core

//#[Swag.PrintBc]
#test
{
    var c: ConcatBuffer
    let s  = c.getSeek()
    c.moveSeek(10)
    @assert(c.count() == 10)
    c.setSeek(s)
    @assert(c.count() == 10)
    c.moveSeek(12)
    @assert(c.count() == 12)
}
