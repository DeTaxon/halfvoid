#import "lib.cp"

glfwInit := !() -> int declare
glfwTerminate := !() -> void declare

glfwSetErrorCallback := !(void^ call) -> void declare
glfwSetKeyCallback := !(void^ win, void^ call) ->void declare

glfwCreateWindow := !(int w,int h, char^ name,void^ monitor, void^ share) -> void^ declare
glfwWindowHint := !(int what, int to) -> void declare
glfwDestroyWindow := !(void^ wind) -> void declare

glfwMakeContextCurrent := !(void^ wind) -> void declare
glfwGetProcAddress := !(char^ proc) -> void^ declare

glfwWindowShouldClose := !(void^ wind) -> bool declare
glfwPollEvents := !() -> void declare

glfwSwapBuffers := !(void^ win) -> void declare
glfwSwapInterval := !(int num) -> void declare

GLFW_KEY_0               :=    48
GLFW_KEY_9               :=    57
GLFW_KEY_A               :=    65
GLFW_KEY_Z               :=    90
GLFW_KEY_LEFT_BRACKET    :=    91  ///* [ */
GLFW_KEY_BACKSLASH       :=    92  ///* \ */
GLFW_KEY_RIGHT_BRACKET   :=    93  ///* ] */
GLFW_KEY_GRAVE_ACCENT    :=    96  ///* ` */
GLFW_KEY_SPACE           :=    32
GLFW_KEY_APOSTROPHE      :=    39  ///* ' */
GLFW_KEY_COMMA           :=    44  ///* , */
GLFW_KEY_MINUS           :=    45  ///* - */
GLFW_KEY_PERIOD          :=    46  ///* . */
GLFW_KEY_SLASH           :=    47  ///* / */
GLFW_KEY_SEMICOLON       :=    59  ///* ; */
GLFW_KEY_EQUAL           :=    61  ///* = */

GLFW_FALSE    := 	0
GLFW_RELEASE  := 	0
GLFW_PRESS    := 	1
GLFW_REPEAT   := 	2
