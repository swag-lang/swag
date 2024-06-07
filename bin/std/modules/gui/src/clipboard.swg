#global namespace Clipboard
using Serialization

var g_ValueFormat: ValueFormat

// Add whatever value with a given 'type'
public func addTypedValue(value: *void, type: typeinfo)
{
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
