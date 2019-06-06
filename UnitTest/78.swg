#unittest module module78;
#unittest pass semantic;

type str: string;
var a: str = "coucou";

#unittest error;
var b: str = '1';
#unittest error;
var c: str = 1;


