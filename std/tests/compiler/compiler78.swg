#module compiler78
#unittest pass semantic;

typealias str: string;
var a: str = "coucou";

#unittest error;
var b: str = '1';
#unittest error;
var c: str = 1;


