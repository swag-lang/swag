#global export
#global namespace Serialization

interface ISerialize
{
    read:           mtd(u32, IDecoder)->bool throw
    write:          mtd(u32, IEncoder)->bool
    readElement:    mtd(u32, string, typeinfo, *void, IDecoder)->bool throw
    writeElement:   mtd(u32, string, typeinfo, const *void, IEncoder)->bool
}
