using Core

public interface ICodec
{
    mtd canDecode(encoding: SoundFileEncoding)->bool
    mtd init(srcBuffer: ^void, srcLength: u64)->u64 throw
    mtd decode(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{write, read: u64} throw
}

struct CodecDesc
{
    type:   const *Swag.TypeInfoStruct
    itf:    ICodec
}

var g_Codecs: Array'CodecDesc

// Register a codec
public func(T) addCodec()
{
    #assert Reflection.hasInterface(@typeof(T), ICodec)

    var codec: CodecDesc = undefined
    codec.type = @typeof(T)
    codec.itf = @mkinterface(null, @typeof(T), ICodec)
    g_Codecs.add(codec)
}

// Find the codec that can treat the given encoding
func getCodec(encoding: SoundFileEncoding)->*CodecDesc
{
    var found: *CodecDesc
    visit &codec : g_Codecs
    {
        if codec.itf.canDecode(encoding)
        {
            found = codec
            break
        }
    }

    return found
}

struct Codec
{
    srcEncoding: SoundFileEncoding
    dstEncoding: SoundFileEncoding
}


// Initialize predefined codecs
#init
{
    addCodec'(CodecPcm)()
}
