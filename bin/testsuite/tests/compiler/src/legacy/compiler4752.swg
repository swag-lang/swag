#global testerror "Err0296"
struct Driver {}
var g_Driver: [2] *Driver

public func toto4751()->*Driver => g_Driver[0]
