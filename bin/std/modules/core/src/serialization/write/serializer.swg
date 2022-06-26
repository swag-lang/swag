#global namespace Serialization.Write
#global public
using Swag

interface ISerializer
{
    startWrite:         func(self, *ConcatBuffer)
    startElement:       func(self, const *Swag.TypeValue)
    endElement:         func(self, const *Swag.TypeValue)
    startValue:         func(self, typeinfo)
    endValue:           func(self, typeinfo)
    startStruct:        func(self, typeinfo, const *u8)->bool
    endStruct:          func(self)
    startSequence:      func(self, typeinfo, uint, const *u8)->bool
    endSequence:        func(self)
    writeBool:          func(self, bool)
}

struct Serializer
{
    output: *ConcatBuffer
}

impl ISerializer for Serializer
{
    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
    }

    mtd startElement(elem: const *Swag.TypeValue)       {}
    mtd endElement(elem: const *Swag.TypeValue)         {}
    mtd startSequence(typeElem: typeinfo, countElem: uint, data: const *u8) => false
    mtd endSequence()                                   {}
    mtd startStruct(type: typeinfo, data: const *u8)    => false
    mtd endStruct()                                     {}
    mtd startValue(type: typeinfo)                      {}
    mtd endValue(type: typeinfo)                        {}

    mtd writeBool(value: bool) {}
}

impl Serializer
{
    mtd initWrite(buffer: *ConcatBuffer)
    {
        output = buffer
    }
}