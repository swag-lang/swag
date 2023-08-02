using Audio

#test
{
    var x: SoundWav
    assume x.read(sndPath("snd1.wav"))
    @assert(x.channelCount == 2)
    @assert(x.dataSize == 216320)
}