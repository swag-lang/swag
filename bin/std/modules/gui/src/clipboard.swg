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
    trycatch
    {
        encode.start(&buf)
        encode.writeValue(value, type)
        encode.end()
    }

    buf.makeLinear()
    addTypedData(g_ValueFormat, type, assume buf.toSlice())
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

    return hasTypedData(g_ValueFormat, type)
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

    let data = getTypedData(g_ValueFormat, type)

    var decoder: Decoder'Read.TagBin
    trycatch
    {
        decoder.start(data.toSlice())
        decoder.readTypedContent(value, type)
        decoder.end()
    }

    return true
}

#init
{
    g_ValueFormat = catch registerFormat("Swag.Clipboard.TypeValue")
}
