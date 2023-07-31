using Audio

#test
{
    var x: SoundWav
    assume x.read(sndPath("snd1.wav"))
    @assert(x.infos.channelCount == 2)
    @assert(x.infos.dataSize == 216320)
}