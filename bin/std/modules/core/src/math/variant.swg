#global export
#global namespace Math

struct(T: const [..] typeinfo) Variant
{
    type:              typeinfo
    buffer:            [Reflection.maxSizeOf(T)] u8
    const AllFlags     = Reflection.orFlags(T)
}

impl Variant
{
    #[Swag.ConstExpr]
    func checkValidType(t: const [..] typeinfo, tn: typeinfo)->bool
    {
        if Slice.contains(t, tn):
            return true
        @compilererror(Format.toString("type [[%]] is not part of the variant", tn.fullname), #location(tn))
        return false
    }

    mtd opDrop()
    {
        drop()
    }

    mtd(TN) opAffect(other: TN)
    {
        set'TN(other)
    }

    mtd(TN) opEquals(other: TN)->bool
        where<call> checkValidType(T, TN)
    {
        Debug.assert(type == TN, "requested type does not match the variant current type")
        return dref cast(*TN) buffer == other
    }

    mtd(TN) opCmp(other: TN)->s32
        where<call> checkValidType(T, TN)
    {
        Debug.assert(type == TN, "requested type does not match the variant current type")
        return dref cast(*TN) buffer <=> other
    }

    mtd opPostCopy()
    {
        #if AllFlags & .HasPostCopy
        {
            if type and Reflection.hasPostCopy(type)
            {
                let typeStruct = cast(const *Swag.TypeInfoStruct) type
                typeStruct.opPostCopy(buffer)
            }
        }
    }

    mtd opPostMove()
    {
        #if AllFlags & .HasPostMove
        {
            if type and Reflection.hasPostMove(type)
            {
                let typeStruct = cast(const *Swag.TypeInfoStruct) type
                typeStruct.opPostMove(buffer)
            }
        }
    }

    // Drop the variant content (if necessary)
    mtd drop()
    {
        #if AllFlags & .HasDrop
        {
            if type and Reflection.hasDrop(type)
            {
                let typeStruct = cast(const *Swag.TypeInfoStruct) type
                typeStruct.opDrop(buffer)
                type = null
            }
        }
    }

    // Set the variant value
    mtd(TN) set(value: TN)
        where<call> checkValidType(T, TN)
    {
        // Drop previous value if necessary
        drop()

        type                  = TN
        dref cast(*TN) buffer = #nodrop value
    }

    // Get the variant value of the given type
    // Will assert if the current type does not match
    mtd(TN) get()->TN
        where<call> checkValidType(T, TN)
    {
        Debug.assert(type == TN, "requested type does not match the variant current type")
        return dref cast(*TN) buffer
    }
}
