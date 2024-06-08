#global namespace Clipboard
using Serialization

var g_ValueFormat: ValueFormat

// Add whatever value with a given 'type'
public func addTypedValue(value: *void, type: typeinfo)
{
    switch type
    {
    case string:
        return addString(dref cast(*string) value)
    case Core.String:
        let str = cast(*Core.String) value
        return addString(str.toString())
    case Pixel.Image:
        return addImage(dref cast(*Pixel.Image) value)
    }

    var buf:    ConcatBuffer
    var encode: Encoder'Write.TagBin

    buf.addNative(type.crc32)
    trycatch
    {
        encode.start(&buf)
        encode.writeValue(value, type)
        encode.end()
    }

    buf.makeLinear()
    addData(g_ValueFormat, buf.toSlice())
}

// Get whatever value with a given 'type'
public func hasTypedValue(type: typeinfo)->bool
{
    switch type
    {
    case string, Core.String:
        return hasString()
    case Pixel.Image:
        return hasImage()
    }

    if !hasFormat(g_ValueFormat) do
        return false

    let data   = getData(g_ValueFormat, 4)
    let ptrCrc = cast(*u32) data.buffer
    return dref ptrCrc == type.crc32
}

// Get whatever value with a given 'type'
public func getTypedValue(value: *void, type: typeinfo)->bool
{
    if !hasTypedValue(type) do
        return false

    switch type
    {
    case Core.String:
        dref cast(*Core.String) value = getString()
        return true
    case Pixel.Image:
        dref cast(*Pixel.Image) value = getImage()
        return true
    }

    let data = getData(g_ValueFormat)

    var decoder: Decoder'Read.TagBin
    trycatch
    {
        decoder.start(data[4..])
        decoder.readValue(value, type)
        decoder.end()
    }

    return true
}

#init
{
    g_ValueFormat = catch registerFormat("Swag.Clipboard.TypedValue")
}
