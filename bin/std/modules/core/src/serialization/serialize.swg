#global export
#global namespace Serialization

interface ISerialize
{
    read:           mtd(IDecoder)->bool throw
    write:          mtd(IEncoder)->bool
    readElement:    mtd(string, typeinfo, *void, IDecoder)->bool throw
    writeElement:   mtd(string, typeinfo, const *void, IEncoder)->bool
}
