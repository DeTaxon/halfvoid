#import "lib.cp"

EGL_ALPHA_SIZE                    := 0x3021
EGL_BAD_ACCESS                    := 0x3002
EGL_BAD_ALLOC                     := 0x3003
EGL_BAD_ATTRIBUTE                 := 0x3004
EGL_BAD_CONFIG                    := 0x3005
EGL_BAD_CONTEXT                   := 0x3006
EGL_BAD_CURRENT_SURFACE           := 0x3007
EGL_BAD_DISPLAY                   := 0x3008
EGL_BAD_MATCH                     := 0x3009
EGL_BAD_NATIVE_PIXMAP             := 0x300A
EGL_BAD_NATIVE_WINDOW             := 0x300B
EGL_BAD_PARAMETER                 := 0x300C
EGL_BAD_SURFACE                   := 0x300D
EGL_BLUE_SIZE                     := 0x3022
EGL_BUFFER_SIZE                   := 0x3020
EGL_CONFIG_CAVEAT                 := 0x3027
EGL_CONFIG_ID                     := 0x3028
EGL_CORE_NATIVE_ENGINE            := 0x305B
EGL_DEPTH_SIZE                    := 0x3025
//EGL_DONT_CARE                     := -1
EGL_DRAW                          := 0x3059
EGL_EXTENSIONS                    := 0x3055
EGL_FALSE                         := 0
EGL_GREEN_SIZE                    := 0x3023
EGL_HEIGHT                        := 0x3056
EGL_LARGEST_PBUFFER               := 0x3058
EGL_LEVEL                         := 0x3029
EGL_MAX_PBUFFER_HEIGHT            := 0x302A
EGL_MAX_PBUFFER_PIXELS            := 0x302B
EGL_MAX_PBUFFER_WIDTH             := 0x302C
EGL_NATIVE_RENDERABLE             := 0x302D
EGL_NATIVE_VISUAL_ID              := 0x302E
EGL_NATIVE_VISUAL_TYPE            := 0x302F
EGL_NONE                          := 0x3038
EGL_NON_CONFORMANT_CONFIG         := 0x3051
EGL_NOT_INITIALIZED               := 0x3001
//EGL_NO_CONTEXT                    := null 
//EGL_NO_DISPLAY                    := null 
//EGL_NO_SURFACE                    := null 
EGL_PBUFFER_BIT                   := 0x0001
EGL_PIXMAP_BIT                    := 0x0002
EGL_READ                          := 0x305A
EGL_RED_SIZE                      := 0x3024
EGL_SAMPLES                       := 0x3031
EGL_SAMPLE_BUFFERS                := 0x3032
EGL_SLOW_CONFIG                   := 0x3050
EGL_STENCIL_SIZE                  := 0x3026
EGL_SUCCESS                       := 0x3000
EGL_SURFACE_TYPE                  := 0x3033
EGL_TRANSPARENT_BLUE_VALUE        := 0x3035
EGL_TRANSPARENT_GREEN_VALUE       := 0x3036
EGL_TRANSPARENT_RED_VALUE         := 0x3037
EGL_TRANSPARENT_RGB               := 0x3052
EGL_TRANSPARENT_TYPE              := 0x3034
EGL_TRUE                          := 1
EGL_VENDOR                        := 0x3053
EGL_VERSION                       := 0x3054
EGL_WIDTH                         := 0x3057
EGL_WINDOW_BIT                    := 0x0004

//EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig (EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
//EGLAPI EGLBoolean EGLAPIENTRY eglCopyBuffers (EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target);
eglCreateContext := !(void^ dpy, void^ config, void^ share_context, s32^ attrib_list) -> void^ declare
//EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurface (EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list);
//EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface (EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list);
eglCreateWindowSurface := !(void^ dpy, void^config, void^ win, s32^ attrib_list) -> void^ declare
//EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext (EGLDisplay dpy, EGLContext ctx);
//EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface (EGLDisplay dpy, EGLSurface surface);
eglGetConfigAttrib := !(void^ dpy, void^ config, s32 attribute, s32^ value) -> int declare
eglGetConfigs := !(void^ dpy, s32^ configs, s32 config_size, s32^ num_config) -> int declare
//EGLAPI EGLDisplay EGLAPIENTRY eglGetCurrentDisplay (void);
//EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface (EGLint readdraw);
eglGetDisplay := !(void^ display_id) -> void^ declare
//EGLAPI EGLint EGLAPIENTRY eglGetError (void);
//EGLAPI __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress (const char *procname);
eglInitialize := !(void^ dpy, s32^ major, s32^ minor) -> int declare
eglMakeCurrent := !(void^ dpy, void^ draw, void^ read, void^ ctx) -> int declare
//EGLAPI EGLBoolean EGLAPIENTRY eglQueryContext (EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value);
//EGLAPI const char *EGLAPIENTRY eglQueryString (EGLDisplay dpy, EGLint name);
//EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value);
eglSwapBuffers := !(void^ dpy, void^ surface) -> int declare
//EGLAPI EGLBoolean EGLAPIENTRY eglTerminate (EGLDisplay dpy);
//EGLAPI EGLBoolean EGLAPIENTRY eglWaitGL (void);
//EGLAPI EGLBoolean EGLAPIENTRY eglWaitNative (EGLint engine);

EGL_ALPHA_FORMAT                  := 0x3088
EGL_ALPHA_FORMAT_NONPRE           := 0x308B
EGL_ALPHA_FORMAT_PRE              := 0x308C
EGL_ALPHA_MASK_SIZE               := 0x303E
EGL_BUFFER_PRESERVED              := 0x3094
EGL_BUFFER_DESTROYED              := 0x3095
EGL_CLIENT_APIS                   := 0x308D
EGL_COLORSPACE                    := 0x3087
EGL_COLORSPACE_sRGB               := 0x3089
EGL_COLORSPACE_LINEAR             := 0x308A
EGL_COLOR_BUFFER_TYPE             := 0x303F
EGL_CONTEXT_CLIENT_TYPE           := 0x3097
EGL_DISPLAY_SCALING               := 10000
EGL_HORIZONTAL_RESOLUTION         := 0x3090
EGL_LUMINANCE_BUFFER              := 0x308F
EGL_LUMINANCE_SIZE                := 0x303D
EGL_OPENGL_ES_BIT                 := 0x0001
EGL_OPENVG_BIT                    := 0x0002
EGL_OPENGL_ES_API                 := 0x30A0
EGL_OPENVG_API                    := 0x30A1
EGL_OPENVG_IMAGE                  := 0x3096
EGL_PIXEL_ASPECT_RATIO            := 0x3092
EGL_RENDERABLE_TYPE               := 0x3040
EGL_RENDER_BUFFER                 := 0x3086
EGL_RGB_BUFFER                    := 0x308E
EGL_SINGLE_BUFFER                 := 0x3085
EGL_SWAP_BEHAVIOR                 := 0x3093
//EGL_UNKNOWN                       := -1
EGL_VERTICAL_RESOLUTION           := 0x3091


EGL_CONFORMANT                    := 0x3042
EGL_CONTEXT_CLIENT_VERSION        := 0x3098
EGL_MATCH_NATIVE_PIXMAP           := 0x3041
EGL_OPENGL_ES2_BIT                := 0x0004
EGL_VG_ALPHA_FORMAT               := 0x3088
EGL_VG_ALPHA_FORMAT_NONPRE        := 0x308B
EGL_VG_ALPHA_FORMAT_PRE           := 0x308C
EGL_VG_ALPHA_FORMAT_PRE_BIT       := 0x0040
EGL_VG_COLORSPACE                 := 0x3087
EGL_VG_COLORSPACE_sRGB            := 0x3089
EGL_VG_COLORSPACE_LINEAR          := 0x308A
EGL_VG_COLORSPACE_LINEAR_BIT      := 0x0020

