var CPT: s32 = ?

struct(T: const [..] typeinfo) AA
{
    #run CPT += 1
}

#test
{
    var x: AA'(@[s32, bool])
    var y: AA'(@[s32, bool])
    var z: AA'(@[bool, bool])
    @assert(CPT == 2)
}