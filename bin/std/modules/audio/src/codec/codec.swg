using Core

var g_Codecs: Array'CodecDesc

public interface ICodec
{
    mtd canDecode(encoding: SoundFileEncoding)->bool
    mtd init(srcBuffer: ^void, srcLength: u64)->u64 throw
    mtd decode(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{write, read: u64} throw
}

public struct CodecDesc
{
    type: const *Swag.TypeInfoStruct
    itf:  ICodec
}

// Register a codec
public func(T) addCodec()
{
    #assert Reflection.hasInterface(@typeof(T), ICodec)

    var desc: CodecDesc = undefined
    desc.type = @typeof(T)
    desc.itf = @mkinterface(null, @typeof(T), ICodec)
    g_Codecs.add(desc)
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

// Base struct for codecs
public struct Codec
{
    srcEncoding: SoundFileEncoding
    dstEncoding: SoundFileEncoding
    type:        const *Swag.TypeInfoStruct
}

// Initialize predefined codecs
#init
{
    addCodec'(CodecPcm)()
}
