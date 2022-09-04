#define NK_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_SOFTWARE_FONT
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_FONT
#include "Nuklear/nuklear.h"

#define NK_RAWFB_IMPLEMENTATION
#include "Nuklear/demo/x11_rawfb/nuklear_rawfb.h"

extern int nk_context_size()
{
	return sizeof(struct nk_context);
}
extern int nk_atlas_size()
{
	return sizeof(struct nk_font_atlas);
}
