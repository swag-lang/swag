using Swag, core

private
{
    struct Decoder
    {
        type:   const *TypeInfoStruct
        itf:    IDecoder
    }

    var g_Decoder: Array'Decoder
}

public struct DecodeOptions
{
    // Decode image informations (width, height...) but do no read pixels
    // Used to fast retreive informations about the image
    decodePixels = true
}

// Interface to decode a buffer
public interface IDecoder
{
    canDecode:  func(self, string)->bool
    decode:     func(self, const [..] u8, DecodeOptions)->Image throw
}

// Register an image decoder
public func(T) addDecoder()
{
    #assert Reflection.hasInterface(@typeof(T), IDecoder)

    var decoder: Decoder = ?
    decoder.type = @typeof(T)
    decoder.itf = @mkinterface(null, @typeof(T), IDecoder)
    g_Decoder.add(decoder)
}

// Load an image from file
public func load(fileName: string, options: DecodeOptions = @{})->Image throw
{
    // Find the decoder that can treat 'fileName'
    var found: *Decoder
    visit *decoder: g_Decoder
    {
        if decoder.itf.canDecode(fileName)
        {
            found = decoder
            break
        }
    }

    if !found throw "no matching image decoder"

    // Read file content
    bytes := try file.readAllBytes(fileName)

    // Create a decoder instance
    ptrDecoder := Memory.new(found.type)
    defer
    {
        if found.type.opDrop
            found.type.opDrop(ptrDecoder);
        Memory.free(ptrDecoder, found.type.sizeof)
    }

    // Decode
    itf := @mkinterface(ptrDecoder, found.type, IDecoder)
    return try itf.decode(bytes.toSlice(), options)
}

// Initialize predefined decoders
#init
{
    addDecoder'(bmp.Decoder)()
    addDecoder'(tga.Decoder)()
    addDecoder'(jpg.Decoder)()
    addDecoder'(png.Decoder)()
}