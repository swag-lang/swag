using Swag, Core

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
impl Image
{
    // Returns 'true' if the given filename has a corresponding decoder
    public func canLoad(fileName: string)->bool
    {
        visit *decoder: g_Decoder
        {
            if decoder.itf.canDecode(fileName)
                return true
        }

        return false
    }

    // Find the decoder that can treat 'fileName'
    func getDecoder(fileName: string)->*Decoder
    {
        var found: *Decoder
        visit *decoder: g_Decoder
        {
            if decoder.itf.canDecode(fileName)
            {
                found = decoder
                break
            }
        }

        return found
    }

    // Decode the given image buffer with the given decoder
    func decode(decoder: *Decoder, bytes: const [..] u8, options: DecodeOptions = @{})->Image throw
    {
        // Create a decoder instance
        ptrDecoder := Memory.new(decoder.type)
        defer
        {
            if decoder.type.opDrop
                decoder.type.opDrop(ptrDecoder)
            Memory.free(ptrDecoder, decoder.type.sizeof)
        }

        // Decode
        itf := @mkinterface(ptrDecoder, decoder.type, IDecoder)
        return try itf.decode(bytes, options)
    }

    // Decode the given image buffer
    public func decode(fileName: string, bytes: const [..] u8, options: DecodeOptions = @{})->Image throw
    {
        found := getDecoder(fileName)
        if !found
            throw "no matching image decoder"
        return try decode(found, bytes, options)
    }

    // Load the given image file
    public func load(fileName: string, options: DecodeOptions = @{})->Image throw
    {
        found := getDecoder(fileName)
        if !found
            throw "no matching image decoder"
        bytes := try File.readAllBytes(fileName)
        return try decode(found, bytes.toSlice(), options)
    }
}

// Initialize predefined decoders
#init
{
    addDecoder'(Bmp.Decoder)()
    addDecoder'(Tga.Decoder)()
    addDecoder'(Jpg.Decoder)()
    addDecoder'(Png.Decoder)()
}