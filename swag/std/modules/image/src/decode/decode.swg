using swag, core

private
{
    var g_Decoder: Array'(const *TypeInfoStruct)
}

// Interface to decode a buffer
public interface IDecoder
{
    canDecode:  func(self, string)->bool
    decode:     func(self, const [..] u8)->Image
}

// Register an image decoder
public func(T) addDecoder()
{
    #assert reflection.hasInterface(@typeof(T), IDecoder)
    g_Decoder.add(@typeof(T))
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
    addDecoder'(bmp.Decoder)()
}