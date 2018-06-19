#import "lib.cp"

glfwInit := !() -> int declare
glfwTerminate := !() -> void declare

glfwCreateWindow := !(int w,int h, char^ name,void^ monitor, void^ share) -> void^ declare
glfwWindowHint := !(int what, int to) -> void declare
glfwDestroyWindow := !(void^ wind) -> void declare

glfwMakeContextCurrent := !(void^ wind) -> void declare

glfwWindowShouldClose := !(void^ wind) -> bool declare
glfwPollEvents := !() -> void declare

