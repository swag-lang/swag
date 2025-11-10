#global skip
struct AA { z: s32; }
struct BB { using p: AA; z: s32; }
struct CC { using p: BB; }

impl CC
{
    func toto(me)
    {
        p.p.z = 0
    }
}
