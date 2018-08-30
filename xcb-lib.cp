xcb_generic_iterator_t := class
{
	data := void^
	rem := int
	index := int
}

xcb_generic_reply_t := class{
	response_type := u8
	pad0 := u8
	sequence := u16
	length := u32
}

xcb_generic_event_t := class
{
	response_type := u8
	pad0 := u8
	sequence := u16
	pad :=  u32[7]
	full_squence := u32
}
xcb_raw_generic_event_t := class
{
	response_type := u8
	pad0 := u8
	sequence := u16
	pad :=  u32[7]
}

xcb_ge_event_t := class
{
	response_type := u8
	pad0 := u8
	sequence := u16
	length := u32
	event_type := u16
	pad1 := u16
	pad := u32[5]
	full_sequence := u32
}

xcb_flush := !(void^)^ -> int
xcb_generate_id := !(void^)^ -> u32
xcb_connect := !(char^,int^)^ -> void^
xcb_disconnect := !(void^)^-> void
