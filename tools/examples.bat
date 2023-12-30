..\bin\swag_devmode clean --ignore-bad-params -w:..\bin\examples %1 %2 %3 %4

..\bin\swag run -w:..\bin\examples -m:genetic --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:opengl1 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:opengl2 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:wnd     --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:captme  --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:draw    --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:img     --args:"swag.test" %1 %2 %3 %4

..\bin\swag run -w:..\bin\examples -m:aoc2015 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:aoc2016 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:aoc2017 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:aoc2018 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:aoc2019 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:aoc2020 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:aoc2021 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:aoc2022 --args:"swag.test" %1 %2 %3 %4
..\bin\swag run -w:..\bin\examples -m:aoc2023 --args:"swag.test" %1 %2 %3 %4