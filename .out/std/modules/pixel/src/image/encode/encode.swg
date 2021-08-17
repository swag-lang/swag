using Swag, Core

private
{
    struct Encoder
    {
        type:   const *TypeInfoStruct
        itf:    IEncoder
    }

    var g_Encoder: Array'Encoder
}

public struct EncodeOptions
{
}

// Interface to decode a buffer
public interface IEncoder
{
    canEncode:  func(self, string)->bool
    encode:     func(self, *ConcatBuffer, Image, EncodeOptions) throw
}

// Register an image encoder
public func(T) addEncoder()
{
    #assert Reflection.hasInterface(@typeof(T), IEncoder)

    var encoder: Encoder = ?
    encoder.type = @typeof(T)
    encoder.itf = @mkinterface(null, @typeof(T), IEncoder)
    g_Encoder.add(encoder)
}

// Save an image to a file
impl Image
{
    public method save(fileName: string, options: EncodeOptions = @{}) throw
    {
        // Find the encoder that can treat 'fileName'
        var found: *Encoder
        visit *encoder: g_Encoder
        {
            if encoder.itf.canEncode(fileName)
            {
                found = encoder
                break
            }
        }

        if !found throw "no matching image encoder"

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
    addEncoder'(Bmp.Encoder)()
}