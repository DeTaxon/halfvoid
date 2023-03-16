#define NK_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_SOFTWARE_FONT
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FIXED_TYPES
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



extern int nke_style_button_size()
{
	return sizeof(struct nk_style_button);
}

extern void nke_style_button_copy_from_context(struct nk_context* ctx,struct nk_style_button* btn_style)
{
	*btn_style = ctx->style.button;
}

extern void nke_style_button_set_color(struct nk_style_button* btn_style,int set_type,struct nk_color set_color)
{
	if (set_type == 0)
	{
		btn_style->normal = nk_style_item_color(set_color);
	}else if (set_type == 1)
	{
		btn_style->hover = nk_style_item_color(set_color);
	}else{
		btn_style->active = nk_style_item_color(set_color);
	}
}
