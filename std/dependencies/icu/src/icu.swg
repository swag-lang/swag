#public
#foreignlib "icuuc"

namespace icu
{
    using swag

    alias UBool = u8
    alias UChar32 = char

    #[foreign("icu")]
    {
        func u_isUWhiteSpace(c: UChar32)->UBool;
    }
}