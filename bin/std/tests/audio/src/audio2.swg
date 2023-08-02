using Audio

#test
{
    var x = assume SoundFile.load(sndPath("snd1.wav"))
    @assert(x.channelCount == 2)
    @assert(x.dataSize == 216320)
}