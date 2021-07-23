#global export
#global namespace Math

struct(T: const [..] typeinfo) Variant
{
    type:   typeinfo
    buffer: [Reflection.maxSizeOf(T)] u8

    const AllFlags = Reflection.orFlags(T)
}

impl Variant
{
    method opDrop()
    {
        drop()
    }

    method(TN) opAffect(other: TN)
    {
        #assert(Slice.contains(T, @typeof(TN)), "type '" ~@stringof(TN)~ "' is not part of the variant")
        set'TN(other)
    }

    method opPostCopy()
    {
        #if AllFlags & .HasPostCopy
        {
            if type and Reflection.hasPostCopy(type)
            {
                typeStruct := cast(const *Swag.TypeInfoStruct) type
                typeStruct.opPostCopy(buffer)
            }
        }
    }

    method opPostMove()
    {
        #if AllFlags & .HasPostMove
        {
            if type and Reflection.hasPostMove(type)
            {
                typeStruct := cast(const *Swag.TypeInfoStruct) type
                typeStruct.opPostMove(buffer)
            }
        }
    }

    method drop()
    {
        #if AllFlags & .HasDrop
        {
            if type and Reflection.hasDrop(type)
            {
                typeStruct := cast(const *Swag.TypeInfoStruct) type
                typeStruct.opDrop(buffer)
            }
        }
    }

    // Set the variant value
    method(TN) set(value: TN)
    {
        // Be sure type is supported
        #assert(Slice.contains(T, @typeof(TN)), "type '" ~@stringof(TN)~ "' is not part of the variant")

        // Drop previous value if necessary
        drop()

        type = TN
        dref cast(*TN) buffer =,nodrop value
    }

    // Get the variant value of the given type
    // Will assert if the current type does not match
    method(TN) get()->TN
    {
        // Be sure type is supported
        #assert(Slice.contains(T, @typeof(TN)), "type '" ~@stringof(TN)~ "' is not part of the variant")
        Debug.assert(type == TN, "requested type does not match the variant current type")
        return dref cast(*TN) buffer
    }
}