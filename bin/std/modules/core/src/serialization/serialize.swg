#global export
#global namespace Serialization
using Swag

interface ISerialize
{
    read:           mtd(IDecoder)->bool throw
    write:          mtd(IEncoder)->bool
    readElement:    mtd(TypeValue, *void, IDecoder)->bool throw
    writeElement:   mtd(TypeValue, const *void, IEncoder)->bool
}
