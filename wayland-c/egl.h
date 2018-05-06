#ifndef __egl_h_
#define __egl_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

/*
** Copyright (c) 2013-2017 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
/*
** This header is generated from the Khronos OpenGL / OpenGL ES XML
** API Registry. The current version of the Registry, generator scripts
** used to make the header, and the header can be found at
**   http://www.khronos.org/registry/egl
**
** Khronos $Git commit SHA1: a732b061e7 $ on $Git commit date: 2017-06-17 23:27:53 +0100 $
*/

#include <EGL/eglplatform.h>

/* Generated on date 20170627 */

/* Generated C header for:
 * API: egl
 * Versions considered: .*
 * Versions emitted: .*
 * Default extensions included: None
 * Additional extensions included: _nomatch_^
 * Extensions removed: _nomatch_^
 */

#ifndef EGL_VERSION_1_0
#define EGL_VERSION_1_0 1
typedef unsigned int EGLBoolean;
typedef void *EGLDisplay;
#include <KHR/khrplatform.h>
#include <EGL/eglplatform.h>
typedef void *EGLConfig;
typedef void *EGLSurface;
typedef void *EGLContext;
typedef void (*__eglMustCastToProperFunctionPointerType)(void);

#endif /* EGL_VERSION_1_0 */

#ifndef EGL_VERSION_1_1
#define EGL_VERSION_1_1 1
#define EGL_BACK_BUFFER                   0x3084
#define EGL_BIND_TO_TEXTURE_RGB           0x3039
#define EGL_BIND_TO_TEXTURE_RGBA          0x303A
#define EGL_CONTEXT_LOST                  0x300E
#define EGL_MIN_SWAP_INTERVAL             0x303B
#define EGL_MAX_SWAP_INTERVAL             0x303C
#define EGL_MIPMAP_TEXTURE                0x3082
#define EGL_MIPMAP_LEVEL                  0x3083
#define EGL_NO_TEXTURE                    0x305C
#define EGL_TEXTURE_2D                    0x305F
#define EGL_TEXTURE_FORMAT                0x3080
#define EGL_TEXTURE_RGB                   0x305D
#define EGL_TEXTURE_RGBA                  0x305E
#define EGL_TEXTURE_TARGET                0x3081
EGLAPI EGLBoolean EGLAPIENTRY eglBindTexImage (EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGLAPI EGLBoolean EGLAPIENTRY eglReleaseTexImage (EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGLAPI EGLBoolean EGLAPIENTRY eglSurfaceAttrib (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval (EGLDisplay dpy, EGLint interval);
#endif /* EGL_VERSION_1_1 */

#ifndef EGL_VERSION_1_2
#define EGL_VERSION_1_2 1
typedef unsigned int EGLenum;
typedef void *EGLClientBuffer;
EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI (EGLenum api);
EGLAPI EGLenum EGLAPIENTRY eglQueryAPI (void);
EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer (EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglReleaseThread (void);
EGLAPI EGLBoolean EGLAPIENTRY eglWaitClient (void);
#endif /* EGL_VERSION_1_2 */

#ifndef EGL_VERSION_1_3
#define EGL_VERSION_1_3 1
#endif /* EGL_VERSION_1_3 */

#ifndef EGL_VERSION_1_4
#define EGL_VERSION_1_4 1
#define EGL_DEFAULT_DISPLAY               EGL_CAST(EGLNativeDisplayType,0)
#define EGL_MULTISAMPLE_RESOLVE_BOX_BIT   0x0200
#define EGL_MULTISAMPLE_RESOLVE           0x3099
#define EGL_MULTISAMPLE_RESOLVE_DEFAULT   0x309A
#define EGL_MULTISAMPLE_RESOLVE_BOX       0x309B
#define EGL_OPENGL_API                    0x30A2
#define EGL_OPENGL_BIT                    0x0008
#define EGL_SWAP_BEHAVIOR_PRESERVED_BIT   0x0400
EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext (void);
#endif /* EGL_VERSION_1_4 */

#ifndef EGL_VERSION_1_5
#define EGL_VERSION_1_5 1
typedef void *EGLSync;
typedef intptr_t EGLAttrib;
typedef khronos_utime_nanoseconds_t EGLTime;
typedef void *EGLImage;
#define EGL_CONTEXT_MAJOR_VERSION         0x3098
#define EGL_CONTEXT_MINOR_VERSION         0x30FB
#define EGL_CONTEXT_OPENGL_PROFILE_MASK   0x30FD
#define EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY 0x31BD
#define EGL_NO_RESET_NOTIFICATION         0x31BE
#define EGL_LOSE_CONTEXT_ON_RESET         0x31BF
#define EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT 0x00000001
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT 0x00000002
#define EGL_CONTEXT_OPENGL_DEBUG          0x31B0
#define EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE 0x31B1
#define EGL_CONTEXT_OPENGL_ROBUST_ACCESS  0x31B2
#define EGL_OPENGL_ES3_BIT                0x00000040
#define EGL_CL_EVENT_HANDLE               0x309C
#define EGL_SYNC_CL_EVENT                 0x30FE
#define EGL_SYNC_CL_EVENT_COMPLETE        0x30FF
#define EGL_SYNC_PRIOR_COMMANDS_COMPLETE  0x30F0
#define EGL_SYNC_TYPE                     0x30F7
#define EGL_SYNC_STATUS                   0x30F1
#define EGL_SYNC_CONDITION                0x30F8
#define EGL_SIGNALED                      0x30F2
#define EGL_UNSIGNALED                    0x30F3
#define EGL_SYNC_FLUSH_COMMANDS_BIT       0x0001
#define EGL_FOREVER                       0xFFFFFFFFFFFFFFFFull
#define EGL_TIMEOUT_EXPIRED               0x30F5
#define EGL_CONDITION_SATISFIED           0x30F6
#define EGL_NO_SYNC                       EGL_CAST(EGLSync,0)
#define EGL_SYNC_FENCE                    0x30F9
#define EGL_GL_COLORSPACE                 0x309D
#define EGL_GL_COLORSPACE_SRGB            0x3089
#define EGL_GL_COLORSPACE_LINEAR          0x308A
#define EGL_GL_RENDERBUFFER               0x30B9
#define EGL_GL_TEXTURE_2D                 0x30B1
#define EGL_GL_TEXTURE_LEVEL              0x30BC
#define EGL_GL_TEXTURE_3D                 0x30B2
#define EGL_GL_TEXTURE_ZOFFSET            0x30BD
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x30B3
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x30B4
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x30B5
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x30B6
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x30B7
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x30B8
#define EGL_IMAGE_PRESERVED               0x30D2
#define EGL_NO_IMAGE                      EGL_CAST(EGLImage,0)
EGLAPI EGLSync EGLAPIENTRY eglCreateSync (EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglDestroySync (EGLDisplay dpy, EGLSync sync);
EGLAPI EGLint EGLAPIENTRY eglClientWaitSync (EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout);
EGLAPI EGLBoolean EGLAPIENTRY eglGetSyncAttrib (EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value);
EGLAPI EGLImage EGLAPIENTRY eglCreateImage (EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglDestroyImage (EGLDisplay dpy, EGLImage image);
EGLAPI EGLDisplay EGLAPIENTRY eglGetPlatformDisplay (EGLenum platform, void *native_display, const EGLAttrib *attrib_list);
EGLAPI EGLSurface EGLAPIENTRY eglCreatePlatformWindowSurface (EGLDisplay dpy, EGLConfig config, void *native_window, const EGLAttrib *attrib_list);
EGLAPI EGLSurface EGLAPIENTRY eglCreatePlatformPixmapSurface (EGLDisplay dpy, EGLConfig config, void *native_pixmap, const EGLAttrib *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglWaitSync (EGLDisplay dpy, EGLSync sync, EGLint flags);
#endif /* EGL_VERSION_1_5 */

#ifdef __cplusplus
}
#endif

#endif
