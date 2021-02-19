using core

private var g_Decoder: Array'typeinfo

// Interface to decode a buffer
public interface IDecoder
{
    decode: func(self, const [..] u8)->Image
}

// Register an iamge decoder
public func addDecoder(decoder: typeinfo)
{
    debug.assert(reflection.hasInterface(decoder, IDecoder))
    g_Decoder.add(decoder)
}

// Load an image from file
public func load(fileName: string)->Image
{
    var result: retval
    return result
}

// Initialize predefined decoders
#init
{
    addDecoder(@typeof(bmp.Decoder))
}