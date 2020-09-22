struct ConvertFormatInt
{
    unsignedValue:  u64
//    #[swag.offset("unsignedValue")]
    signedValue:    s64
    bits:           u32 = 32
    base:           u32 = 10
    signed:         bool
}

//#[swag.printbc]
#test
{
    var format: ConvertFormatInt{
                    signedValue: 5,
                    bits: 8,
                    signed: true
                }
    @assert(format.signedValue == 5)
    @assert(format.bits == 8)
    @assert(format.signed == true)
}