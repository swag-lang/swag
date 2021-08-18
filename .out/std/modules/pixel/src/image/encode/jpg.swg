#global namespace Jpg
using Core

struct APP0infotype
{
    marker: u16
    length: u16
    JFIFsignature: [5] u8
    versionhi: u8
    versionlo: u8
    xyunits: u8
    xdensity: u16
    ydensity: u16
    thumbnwidth: u8
    thumbnheight: u8
}

struct SOF0infotype
{
    marker: u16
    length: u16
    precision: u8
    height: u16
    width: u16
    nrofcomponents: u8
    IdY: u8
    HVY: u8
    QTY: u8
    IdCb: u8
    HVCb: u8
    QTCb: u8
    IdCr: u8
    HVCr: u8
    QTCr: u8
}

struct SOSinfotype
{
    marker: u16
    length: u16
    nrofcomponents: u8
    IdY: u8
    HTY: u8
    IdCb: u8
    HTCb: u8
    IdCr: u8
    HTCr: u8
    Ss: u8
    Se: u8
    Bf: u8
}

//const APP0info: APP0infotype = @{0xFFE0, 16, ["J"'u8, "F"'u8, "I"'u8, "F"'u8, 0], 1, 1, 0, 1, 1, 0, 0}

public struct Encoder
{
}

public impl IEncoder for Encoder
{
    method canEncode(fileName: string)->bool
    {
        ext := Path.getExtensionLowerCase(fileName)
        return ext == ".jpg" or ext == ".jpeg"
    }

    method encode(result: *ConcatBuffer, image: Image, options: EncodeOptions) throw
    {
    }
}