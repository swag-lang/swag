//#[Swag.printbc]
#run
{
    var c: *u8 = null
    c += 0xFF00
    var usedBuckets: u32
    usedBuckets = c ? 1 : 0
    @assert(usedBuckets == 1)
}