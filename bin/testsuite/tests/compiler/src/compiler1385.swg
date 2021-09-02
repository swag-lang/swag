enum Key
{
    None
    B
}

alias VirtualKey = u32
var g_mapVKeyToKey: [256] u8

func VKeyToKey(vkey: VirtualKey)-> Key
{
    if(vkey > 255) return Key.None
    return cast(Key) g_mapVKeyToKey[cast(u8) vkey]
}