namespace time;

// Provides a set of methods and properties that you can use to accurately measure elapsed time
public struct Stopwatch
{
    isRunning:		bool
    startTimeStamp:	u64
    elapsedTicks:	u64
}

public impl Stopwatch
{
    // Starts, or resumes, measuring elapsed time for an interval
    func start(using self)
    {
        if !isRunning
        {
            isRunning = true
            startTimeStamp = nowPrecise()
        }
    }

    // Stops measuring elapsed time for an interval
    func stop(using self)
    {
        if isRunning
        {
            endTimeStamp := nowPrecise()
            elapsedTicksPeriod := endTimeStamp - startTimeStamp
            elapsedTicks += elapsedTicksPeriod
            isRunning = false
        }
    }

    // Stops time interval measurement and resets the elapsed time to zero
    func reset(using self)
    {
        elapsedTicks = 0
        startTimeStamp = 0
        isRunning = false
    }

    // Stops time interval measurement, resets the elapsed time to zero, and starts measuring elapsed time
    func restart(using self)
    {
        elapsedTicks = 0
        startTimeStamp = nowPrecise()
        isRunning = true
    }

    // Gets the total elapsed time in milliseconds
    func elapsedMilliseconds(using self)->u32
    {
        return autocast ((elapsedTicks * 1000) / preciseFrequency())
    }

    // Gets the total elapsed time in microseconds
    func elapsedMicroseconds(using self)->u32
    {
        return autocast ((elapsedTicks * 1000000) / preciseFrequency())
    }
}
