#global export
#global namespace Serialization
using Swag

#[AttrUsage(AttributeUsage.Struct)]
attr Version(v: u32)    // Set the struct version number

#[AttrUsage(AttributeUsage.Struct)]
attr Final()            // The struct does not need versionning

#[AttrUsage(AttributeUsage.Struct|AttributeUsage.StructVariable)]
attr NoSerialize()      // Do not serialize a struct or a field

interface ISerialize
{
    read:           mtd(IDecoder)->bool throw
    write:          mtd(IEncoder)->bool
    readElement:    mtd(TypeValue, *void, IDecoder)->bool throw
    writeElement:   mtd(TypeValue, const *void, IEncoder)->bool
}
