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
    #[Swag.ConstExpr]
    func checkValidType(t: const [..] typeinfo, tn: typeinfo)->bool
    {
        if Slice.contains(t, tn)
            return true
        @errormsg(Format.toString("type '%' is not part of the variant", tn.name), #location(tn))
        return false
    }

    method opDrop()
    {
        drop()
    }

    method(TN) opAffect(other: TN)
    {
        set'TN(other)
    }

    method(TN) opEquals(other: TN)->bool
        #checkif checkValidType(T, TN)
    {
        Debug.assert(type == TN, "requested type does not match the variant current type")
        return dref cast(*TN) buffer == other
    }

    method(TN) opCmp(other: TN)->s32
        #checkif checkValidType(T, TN)
    {
        Debug.assert(type == TN, "requested type does not match the variant current type")
        return dref cast(*TN) buffer <=> other
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

    // Drop the variant content (if necessary)
    method drop()
    {
        #if AllFlags & .HasDrop
        {
            if type and Reflection.hasDrop(type)
            {
                typeStruct := cast(const *Swag.TypeInfoStruct) type
                typeStruct.opDrop(buffer)
                type = null
            }
        }
    }

    // Set the variant value
    method(TN) set(value: TN)
        #checkif checkValidType(T, TN)
    {
        // Drop previous value if necessary
        drop()

        type = TN
        dref cast(*TN) buffer =,nodrop value
    }

    // Get the variant value of the given type
    // Will assert if the current type does not match
    method(TN) get()->TN
        #checkif checkValidType(T, TN)
    {
        Debug.assert(type == TN, "requested type does not match the variant current type")
        return dref cast(*TN) buffer
    }
}