win := void^

Butts := bool[256]

KeyEvent_type := !(int key, int state)^ -> void
 
TCreateWindow := !(int x,int y) -> void declare
TDestroyWindow := !() -> void declare
SetKeyEvent := !(KeyEvent_type type) -> void declare
PullEvents := !() -> void declare

Keys := !(int key, int typo) -> void
{
	printf("%c\n",key)
	Butts[key] =  typo != 0
}


main := !(int argc,string[] argv) -> int 
{
	TCreateWindow(500,500)
	SetKeyEvent(Keys)
	while not Butts['q']
	{
		PullEvents()	
		sleep(1)
	}
	TDestroyWindow()
	return 0
}

