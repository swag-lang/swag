#global testerror "Err0640"
func decode()->{write, read: u64}
{
    func toto()->{write, read: f32}
    {
        return {1.1, 1.0}
    }
    return toto()
}
