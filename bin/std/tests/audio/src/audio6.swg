using Audio

#test
{
    assume Audio.createEngine()
    defer Audio.destroyEngine()

    var x = assume SoundFile.load(sndPath("pacman_eatfruit.wav"), loadDatas = false)
    assume Voice.play(&x)//, playFlags = .Loop)
    Core.Threading.Thread.sleep(2's)
}