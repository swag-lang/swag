#global testerror "Err1"

struct Err1 { x: s32 }

#test
{
    throw Err1{x: 66}
}
