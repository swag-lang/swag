using Swag, Core, Core.Errors

struct ImageEncoder
{
    type:     const *TypeInfoStruct
    itf:      IImageEncoder
}

// Interface to decode a buffer
public interface IImageEncoder
{
    mtd canEncode(filename: string)->bool;
    mtd encode(buf: *ConcatBuffer, img: Image, opt: #null any) throw;
}

public impl Image
{
    internal var encoders: Array'ImageEncoder

    // Register an image encoder
    func(T) addEncoder()
    {
        #assert(Reflection.hasInterface(#typeof(T), IImageEncoder))

        var encoder: ImageEncoder = undefined
        encoder.type = #typeof(T)
        encoder.itf  = @mkinterface(null, #typeof(T), IImageEncoder)
        encoders.add(encoder)
    }

    // Returns 'true' if the given filename has a corresponding encoder
    func canSave(fileName: string)->bool
    {
        foreach &encoder in encoders
        {
            if encoder.itf.canEncode(fileName) do
                return true
        }

        return false
    }

    // Save the image to a file
    mtd save(fileName: string, options: any = null) throw
    {
        Debug.assert(.pixels.buffer != null, "image is invalid")

        // Find the encoder that can treat 'fileName'
        var found: *ImageEncoder
        foreach &encoder in encoders
        {
            if encoder.itf.canEncode(fileName)
            {
                found = encoder
                break
            }
        }

        if !found do
            throw UnsupportedFormatError{"no matching image encoder"}

        // Create an encoder instance
        let ptrEncoder = Memory.new(found.type)
        defer
        {
            if found.type.opDrop do
                found.type.opDrop(ptrEncoder)
            Memory.free(ptrEncoder, cast(u64) found.type.sizeof)
        }

        // Encode
        let itf     = @mkinterface(ptrEncoder, found.type, IImageEncoder)
        var result: ConcatBuffer
        itf.encode(&result, dref me, options)
        File.writeAllBytes(fileName, result)
    }
}

// Initialize predefined encoders
#init
{
    Image.addEncoder'(Bmp.Encoder)()
    Image.addEncoder'(Tga.Encoder)()
    Image.addEncoder'(Jpg.Encoder)()
    Image.addEncoder'(Png.Encoder)()
}
