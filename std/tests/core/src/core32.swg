#import "core"

#test
{
    using core.time
    ticks := nowPrecise() / preciseFrequency()
}