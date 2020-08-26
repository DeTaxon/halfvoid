glfwInit := !() -> bool declare
glfwTerminate := !() -> void declare
glfwCreateWindow := !(int x, int y, char^ Title, int A,int B) -> void^ declare
glfwDestroyWindow := !(void^ W) -> void declare 
glfwMakeContextCurrent := !(void^ W) -> void declare
glfwPollEvents := !() -> void declare
glfwWindowShouldClose := !(void^ W) -> bool declare
glfwSetWindowShouldClose := !(void^ W,bool To) -> bool declare
glfwGetTime := !() -> double declare
glfwSwapInterval := !(int Inter) -> void declare
glfwSwapBuffers := !(void^ W) ->void declare
glfwGetProcAddress := !(char^ Name) -> void^ declare
glfwWindowHint := !(int key,int value) -> void declare

debugCall := type !(int Data,char^ Line)^ -> void
glfwSetErrorCallback := !(debugCall ToCall) -> void declare

keyCall := type !(void^ win, int key, int scancode, int action, int mods)^ -> void
glfwSetKeyCallback := !(void^ win, keyCall ToPress) -> void declare

GLFW_FOCUSED              ::  0x00020001
GLFW_ICONIFIED            ::  0x00020002
GLFW_RESIZABLE            ::  0x00020003
GLFW_VISIBLE              ::  0x00020004
GLFW_DECORATED            ::  0x00020005
GLFW_AUTO_ICONIFY         ::  0x00020006
GLFW_FLOATING             ::  0x00020007

GLFW_RED_BITS             ::  0x00021001
GLFW_GREEN_BITS           ::  0x00021002
GLFW_BLUE_BITS            ::  0x00021003
GLFW_ALPHA_BITS           ::  0x00021004
GLFW_DEPTH_BITS           ::  0x00021005
GLFW_STENCIL_BITS         ::  0x00021006
GLFW_ACCUM_RED_BITS       ::  0x00021007
GLFW_ACCUM_GREEN_BITS     ::  0x00021008
GLFW_ACCUM_BLUE_BITS      ::  0x00021009
GLFW_ACCUM_ALPHA_BITS     ::  0x0002100A
GLFW_AUX_BUFFERS          ::  0x0002100B
GLFW_STEREO               ::  0x0002100C
GLFW_SAMPLES              ::  0x0002100D
GLFW_SRGB_CAPABLE         ::  0x0002100E
GLFW_REFRESH_RATE         ::  0x0002100F
GLFW_DOUBLEBUFFER         ::  0x00021010

GLFW_CLIENT_API             :: 0x00022001
GLFW_CONTEXT_VERSION_MAJOR  :: 0x00022002
GLFW_CONTEXT_VERSION_MINOR  :: 0x00022003
GLFW_CONTEXT_REVISION       :: 0x00022004
GLFW_CONTEXT_ROBUSTNESS     :: 0x00022005
GLFW_OPENGL_FORWARD_COMPAT  :: 0x00022006
GLFW_OPENGL_DEBUG_CONTEXT   :: 0x00022007
GLFW_OPENGL_PROFILE         :: 0x00022008
GLFW_CONTEXT_RELEASE_BEHAVIOR :: 0x00022009

GLFW_OPENGL_API             :: 0x00030001
GLFW_OPENGL_ES_API          :: 0x00030002

GLFW_NO_ROBUSTNESS          ::        0
GLFW_NO_RESET_NOTIFICATION  :: 0x00031001
GLFW_LOSE_CONTEXT_ON_RESET  :: 0x00031002

GLFW_OPENGL_ANY_PROFILE     ::         0
GLFW_OPENGL_CORE_PROFILE    :: 0x00032001
GLFW_OPENGL_COMPAT_PROFILE  :: 0x00032002

GLFW_CURSOR                 :: 0x00033001
GLFW_STICKY_KEYS            :: 0x00033002
GLFW_STICKY_MOUSE_BUTTONS   :: 0x00033003

GLFW_CURSOR_NORMAL          :: 0x00034001
GLFW_CURSOR_HIDDEN          :: 0x00034002
GLFW_CURSOR_DISABLED        :: 0x00034003

GLFW_ANY_RELEASE_BEHAVIOR   ::         0
GLFW_RELEASE_BEHAVIOR_FLUSH :: 0x00035001
GLFW_RELEASE_BEHAVIOR_NONE  :: 0x00035002
