#global skip
#test
{
    assume Audio.createEngine()
    defer Audio.destroyEngine()
}