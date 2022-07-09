#global export

// Represents an owned pointer to a struct, with a dynamic type
struct(T) Object
{
    #assert Reflection.isStruct(T)
    type:   typeinfo
    value:  *T
}

impl Object
{
    mtd opDrop()
    {
        if value
            Memory.delete(value, type)
    }

    #[Swag.Discardable]
    mtd alloc(type: typeinfo)->*T
    {
        @drop(self)
        self.type = #self.type
        self.value = acast Memory.new(#self.type)
        return self.value
    }

    mtd free()
    {
        @drop(self)
        type, value = null
    }
}