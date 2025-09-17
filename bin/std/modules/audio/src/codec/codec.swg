using Core

var g_Codecs: Array'CodecDesc

// Interface to describe a codec
public interface ICodec
{
    mtd canEncode(encoding: SoundFileEncoding)->bool;
    mtd canDecode(encoding: SoundFileEncoding)->bool;
    mtd init(srcBuffer: [*] void, srcLength: u64)->u64 throw;
    mtd decode(destBuffer: [*] void, destLength: u64, srcBuffer: [*] void, srcLength: u64)->{ write, read: u64 } throw;
}

struct CodecDesc
{
    type:     const *Swag.TypeInfoStruct
    itf:      ICodec
}

// Register a codec
public func(T) addCodec()
{
    #assert(Reflection.hasInterface(#typeof(T), ICodec))

    var desc: CodecDesc = undefined
    desc.type = #typeof(T)
    desc.itf  = @mkinterface(null, #typeof(T), ICodec)
    g_Codecs.add(desc)
}

// Find the codec that can treat the given encoding
func getCodec(dstEncoding, srcEncoding: SoundFileEncoding)->#null *CodecDesc
{
    var found: *CodecDesc
    foreach &codec in g_Codecs
    {
        if codec.itf.canEncode(dstEncoding) and codec.itf.canDecode(srcEncoding)
        {
            found = codec
            break
        }
    }

    return found
}

// Base struct for all codec instances
public struct Codec
{
    srcEncoding:     SoundFileEncoding              // The original encoding
    dstEncoding:     SoundFileEncoding              // The requested encoding
    type:            const *Swag.TypeInfoStruct     // The real type of the codec
}

// Initialize predefined codecs
#init
{
    addCodec'CodecPcmToPcm16()
}
