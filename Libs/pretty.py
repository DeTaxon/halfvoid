gdb.inlined-script
import gdb.printing

class StringSpanPretty:
	def __init__(self,val):
		self.val = val
	def display_hint(self):
		return 'string'
	def to_string(self):
		try:
			sz = self.val['itSize']
			if sz == 0:
				return ""
			return self.val['ptr'][::(sz-1)]
		except Exception:
			return "<!error!>"
class ListPretty:
	def __init__(self,val):
		self.val = val
	def display_hint(self):
		return 'array'
	def to_string(self):
		try:
			sz = self.val['itSize']
			if sz == 0:
				return ""
			return self.val['ptr'][::(sz-1)]
		except Exception:
			return "<!error!>"

			
def str_lookup_function(val):
	tg = val.type.tag 
	if tg == None:
		return None
	if tg == "StringSpan":
		return StringSpanPretty(val)
	if tg == "List":
		return ListPretty(val)
	return None
	
def build_pretty_printer():
	pp = gdb.printing.RegexpCollectionPrettyPrinter("my_library")
	pp.add_printer('StringSpan','^StringSpan$',StringSpanPretty)
	return pp
	
gdb.printing.register_pretty_printer(
	gdb.current_objfile(),
	build_pretty_printer()
	)
