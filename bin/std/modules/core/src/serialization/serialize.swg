#global export
#global namespace Serialization
using Swag

// Set the struct version number
#[AttrUsage(AttributeUsage.Struct)]
attr Version(v: u32)

// The struct does not need versionning
#[AttrUsage(AttributeUsage.Struct)]
attr Final()

// Accept this other name for the field
#[AttrUsage(AttributeUsage.StructVariable|AttributeUsage.Multi)]
attr Alias(name: string)

// Do not serialize a struct or a field
#[AttrUsage(AttributeUsage.Struct|AttributeUsage.StructVariable)]
attr NoSerialize()

interface ISerialize
{
    read:           mtd(IDecoder)->bool throw
    write:          mtd(IEncoder)->bool
    readElement:    mtd(TypeValue, *void, IDecoder)->bool throw
    writeElement:   mtd(TypeValue, const *void, IEncoder)->bool
    postSerialize:  mtd(*void, IDecoder)
}
