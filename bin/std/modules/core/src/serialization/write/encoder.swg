#global namespace Serialization
using Swag

public struct Encoder
{
    result:     *ConcatBuffer
    serializer: Write.ISerializer
}

impl Encoder
{
    mtd write(dataType: typeinfo, data: const *u8)
    {
        type := Reflection.makeConcreteAlias(dataType)

        serializer.startValue(type)
        defer serializer.endValue(type)

        if Reflection.isBool(type)
        {
            serializer.writeBool(dref cast(const *bool) data)
            return
        }

        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            treated := serializer.startSequence(typeArr.pointedType, typeArr.count, data)
            defer serializer.endSequence()

            if !treated
            {
                bufArr := cast(const *u8) data
                loop typeArr.count
                {
                    write(typeArr.pointedType, bufArr)
                    bufArr += typeArr.pointedType.sizeof
                }
            }

            return
        }

        if Reflection.isStruct(type)
        {
            typeStruct := cast(const *TypeInfoStruct) type
            treated := serializer.startStruct(typeStruct, data)
            defer serializer.endStruct()

            if !treated
            {
                visit field: typeStruct.fields
                {
                    serializer.startElement(field)
                    write(field.pointedType, data + field.offset)
                    serializer.endElement(field)
                }
            }
        }
    }

    public mtd init(result: *ConcatBuffer, serializer: Write.ISerializer)
    {
        self.result = #self.result
        self.serializer = #self.serializer
        self.serializer.startWrite(#self.result)
    }

    public mtd write(value: any)
    {
        write(@kindof(value), @dataof(value))
    }
}