#global testerror "Err0530"
#test
{
    var bytenew: u8
    var bytepos: s8
    const mask: [4] u16 = [0,0,0,0]
    bytenew |= mask[bytepos]
}