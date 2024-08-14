#global testerror "Err0535"

const SmallSizeInternal = @sizeof(Utf8.size)
struct Utf8
{
    size: u32
    toto: [SmallSizeInternal] u32
}