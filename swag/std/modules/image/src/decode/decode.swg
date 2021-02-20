using swag, core

private
{
    struct Decoder
    {
        type:   const *TypeInfoStruct
        itf:    IDecoder
    }

    var g_Decoder: Array'Decoder
}

// Interface to decode a buffer
public interface IDecoder
{
    canDecode:  func(self, string)->bool
    decode:     func(self, const [..] u8)->Image throw
}

// Register an image decoder
public func(T) addDecoder()
{
    #assert reflection.hasInterface(@typeof(T), IDecoder)

    var decoder: Decoder = ?
    decoder.type = @typeof(T)
    decoder.itf = @mkinterface(null, @typeof(T), IDecoder)
    g_Decoder.add(decoder)
}

// Load an image from file
//#[swag.printbc]
public func load(fileName: string)->Image throw
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

    if !found throw "unknown image format"

    // Read file content
    bytes := try file.readAllBytes(fileName)

    // Create a decoder instance
    ptrDecoder := memory.alloc(found.type.sizeof)
    defer memory.free(ptrDecoder, found.type.sizeof)
    found.type.opInit(ptrDecoder)

    // Decode
    itf := @mkinterface(ptrDecoder, found.type, IDecoder)
    return try itf.decode(bytes)
}

// Initialize predefined decoders
#init
{
    addDecoder'(bmp.Decoder)()
}