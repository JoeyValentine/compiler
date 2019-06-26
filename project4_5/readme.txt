make : compiler all source codes
make clean : clean all object and executable files

if you want to code generate Cminus code,
type ./project4_5 [source file name]
then Program Parse this code and analyze syntax tree.
Finally generate MIPS code file.
if success, generate [source file name].tm file
if you want to execute, spim -file [source file name].tm
