using Swag, Core

private struct Encoder
{
    type:   const *TypeInfoStruct
    itf:    IEncoder
}

public struct EncodeOptions
{
    quality: u8 = 10    // Encoding quality between 1 and 100
}

// Interface to decode a buffer
public interface IEncoder
{
    canEncode:  func(self, string)->bool
    encode:     func(self, *ConcatBuffer, Image, EncodeOptions) throw
}

public impl Image
{
    protected var encoders: Array'Encoder

    // Register an image encoder
    func(T) addEncoder()
    {
        #assert Reflection.hasInterface(@typeof(T), IEncoder)

        var encoder: Encoder = ?
        encoder.type = @typeof(T)
        encoder.itf = @mkinterface(null, @typeof(T), IEncoder)
        encoders.add(encoder)
    }

    // Returns 'true' if the given filename has a corresponding encoder
    func canSave(fileName: string)->bool
    {
        visit *encoder: encoders
        {
            if encoder.itf.canEncode(fileName)
                return true
        }

        return false
    }

    // Save the image to a file
    mtd save(fileName: string, options: EncodeOptions = @{}) throw
    {
        Debug.assert(pixels != null, "image is invalid")

        // Find the encoder that can treat 'fileName'
        var found: *Encoder
        visit *encoder: encoders
        {
            if encoder.itf.canEncode(fileName)
            {
                found = encoder
                break
            }
        }

        if !found
            throw "no matching image encoder"

        // Create an encoder instance
        ptrEncoder := Memory.new(found.type)
        defer
        {
            if found.type.opDrop
                found.type.opDrop(ptrEncoder)
            Memory.free(ptrEncoder, found.type.sizeof)
        }

        // Encode
        itf := @mkinterface(ptrEncoder, found.type, IEncoder)
        var result: ConcatBuffer
        try itf.encode(&result, dref self, options)
        try File.writeAllBytes(fileName, result)
    }
}

// Initialize predefined encoders
#init
{
    Image.addEncoder'(Bmp.Encoder)()
    Image.addEncoder'(Tga.Encoder)()
    Image.addEncoder'(Jpg.Encoder)()
}