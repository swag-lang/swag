#global testerror "Err0619"

struct Point
{
    x:     s32
    y:     s32
}

alias toto = s32

impl Point
{
    func opEquals(me, other: Point)->toto
    {
        return 1
    }
}
