using Swag

//#[Swag.PrintBc]
#test
{
    var ptr = #typeof(const *TypeInfo)

    let ptr1 = cast(const *TypeInfoArray) #typeof(#type [2] s32)
    @assert(ptr1.count == 2)
    @assert(ptr1.pointedType == #typeof(s32))

    let ptr2 = cast(const *TypeInfoArray) #typeof(#type [2, 3] string)
    @assert(ptr2.count == 2)
    @assert(ptr2.totalCount == 6)
    @assert(ptr2.pointedType == #typeof(#type [3] string))
    @assert(ptr2.finalType == #typeof(string))

    let ptr3 = cast(const *TypeInfoSlice) #typeof(#type [..] string)
    @assert(ptr3.pointedType == #typeof(string))
}
