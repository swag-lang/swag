//#global skip
using Core, Audio

#test
{
    var b = assume File.readAllBytes(sndPath("pacman_intermission.ogg"))
    var v: Vorbis
    discard assume v.openPushData(b.buffer, cast(u32) b.count)
    Console.print(v.comments, "\n")
}