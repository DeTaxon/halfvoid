main := !(int argc, char^^ argv) -> int
{
	ins := VkLayerProperties
	ins.specVersion = 3
	printf("test %i\n",VK_STRUCTURE_TYPE_SUBMIT_INFO)
	return 0
}

