#global skip
using Audio

#test
{
    assume Audio.createEngine()
    defer Audio.destroyEngine()
    Audio.setVolume(1)

    var x = assume SoundFile.load(sndPath("pacman_eatfruit.wav"), loadDatas = false)
    assume Voice.play(&x)

    //while Audio.isPlayingSomething() {}
}