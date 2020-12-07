#import "std"

#test
{
    using std.time
    ticks := nowPrecise() / preciseFrequency()
}