using Swag, Core, Core.Errors

struct ImageDecoder
{
    type:     const *TypeInfoStruct
    itf:      IImageDecoder
}

public struct DecodeOptions
{
    // Decode image informations (width, height...) but do no read pixels
    // Used to fast retrieve informations about the image
    decodePixels = true

    // Decode meta datas
    decodeMetaDatas = true
}

// Interface to decode a buffer
public interface IImageDecoder
{
    mtd canDecode(filename: string)->bool;
    mtd decode(buf: const [..] u8, opt: DecodeOptions)->Image throw;
}

impl Image
{
    var decoders: Array'ImageDecoder

    // Find the decoder that can treat 'fileName'
    func getDecoder(fileName: string)->#null *ImageDecoder
    {
        var found: *ImageDecoder
        foreach &decoder in decoders
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
    #[Swag.Overload]
    func decode(decoder: *ImageDecoder, bytes: const [..] u8, options: DecodeOptions = {})->Image throw
    {
        // Create a decoder instance
        let ptrDecoder = Memory.new(decoder.type)
        defer
        {
            if decoder.type.opDrop do
                decoder.type.opDrop(ptrDecoder)
            Memory.free(ptrDecoder, cast(u64) decoder.type.sizeof)
        }

        // Decode
        let itf = @mkinterface(ptrDecoder, decoder.type, IImageDecoder)
        return try itf.decode(bytes, options)
    }
}

// Load an image from file
public impl Image
{
    // Register an image decoder
    func(T) addDecoder()
    {
        #assert(Reflection.hasInterface(#typeof(T), IImageDecoder))

        var decoder: ImageDecoder = undefined
        decoder.type = #typeof(T)
        decoder.itf  = @mkinterface(null, #typeof(T), IImageDecoder)
        decoders.add(decoder)
    }

    // Returns 'true' if the given filename has a corresponding decoder
    func canLoad(fileName: string)->bool
    {
        foreach &decoder in decoders
        {
            if decoder.itf.canDecode(fileName) do
                return true
        }

        return false
    }

    // Decode the given image buffer
    #[Swag.Overload]
    func decode(fileName: string, bytes: const [..] u8, options: DecodeOptions = {})->Image throw
    {
        let found = getDecoder(fileName)
        if !found do
            throw UnsupportedFormatError{"no matching image decoder"}
        return decode(found, bytes, options)
    }

    // Load the given image file
    func load(fileName: string, options: DecodeOptions = {})->Image throw
    {
        let found = getDecoder(fileName)
        if !found do
            throw UnsupportedFormatError{"no matching image decoder"}
        var bytes = File.readAllBytes(fileName)
        return decode(found, bytes.toSlice(), options)
    }
}

// Initialize predefined decoders
#init
{
    Image.addDecoder'(Bmp.Decoder)()
    Image.addDecoder'(Tga.Decoder)()
    Image.addDecoder'(Jpg.Decoder)()
    Image.addDecoder'(Png.Decoder)()
    Image.addDecoder'(Gif.Decoder)()
}
