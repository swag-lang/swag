..\bin\swag clean --script

..\bin\swag run -w:..\bin\examples -m:wnd --script --args:"swag.test"
..\bin\swag run -w:..\bin\examples -m:captme --script --args:"swag.test"

..\bin\swag ..\bin\examples\scripts\mine.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\pong.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\verlet.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\mandelbrot.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\matrix.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\gameoflife.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\clock.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\blob.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\swag.swgs --args:"swag.test"
..\bin\swag ..\bin\examples\scripts\pendulum.swgs --args:"swag.test"
