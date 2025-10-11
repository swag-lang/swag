struct APP0infotype
{
    marker:            u16
    length:            u16
    jfifSignature:     [5] u8
    versionhi:         u8
    versionlo:         u8
    xyunits:           u8
    xdensity:          u16
    ydensity:          u16
    thumbnwidth:       u8
    thumbnheight:      u8
}

const APP0info = APP0infotype{0xFFE0, 16, ['J''u8, 'F''u8, 'I''u8, 'F''u8, 0], 1, 1, 0, 1, 1, 0, 0}

#assert(APP0info.jfifSignature[0] == 'J''u8)
#assert(APP0info.jfifSignature[1] == 'F''u8)
#assert(APP0info.jfifSignature[2] == 'I''u8)
#assert(APP0info.jfifSignature[3] == 'F''u8)
