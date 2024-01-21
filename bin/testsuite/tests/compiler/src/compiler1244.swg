#global testerror "Err0625"

const SmallSizeInternal = @sizeof(Utf8.size)
struct Utf8
{
    size: u32
    toto: [SmallSizeInternal] u32
}