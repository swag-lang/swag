using Swag

#[AttrUsage(AttributeUsage.GlobalVariable)]
attr toto(b: bool);
#[toto(true)]
var a: bool;

#[AttrUsage(AttributeUsage.GlobalVariable)]
attr toto1(b1: bool, b2: bool);
#[toto1(true, false)]
var aa: bool;