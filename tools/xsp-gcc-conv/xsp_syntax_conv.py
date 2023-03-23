#!/usr/bin/python3
# shithouse XSP 68k syntax converter 0.2
# really just awful python script whose only goal is to convert the HAS-syntax
# XSPlib 68000 assembly into something GAS will assemble. This is so we can get
# an object file that modern GCC can link to when cross-compiling/assembling.
#
# in case it isn't clear from this file, I do not work in Python professionally
#
# mike moffitt
import sys
import re

def horrible_xsp_flag_hack(line):
	if "cmpi.b" not in line:
		return line
	if "XSP_flg" not in line:
		return line
	return line.replace("XSP_flg", "(XSP_flg).l")

g_divy_sub_hack_on = False

def div_y_sub_hack(line):
	global g_divy_sub_hack_on

	if g_divy_sub_hack_on:
		if ".endm" in line:
			g_divy_sub_hack_on = False
			return line
		line = line.replace('else:', 'reg_less:')
		line = line.replace('else', 'reg_less')
		return line
	else:
		if "ADJUST_DIV_Y_SUB" not in line:
			return line
		g_divy_sub_hack_on = True
		return line

def pr_protect_hack(line):
	if "PR_PROTECT" in line:
		line = line.replace("\t0", "\t")
		line = line.replace(",0", ",")
	return line

def parse_dcb(line):
	if "dcb." in line:
		tokens = re.split('\t| |,', line)
		while "" in tokens:
			tokens.remove("")
		# print(tokens)
		dcbidx = 0
		for i in range(0, len(tokens)):
			token = tokens[i]
			if "dcb." in token:
				dcbidx = i
				break
		
		sizechr = 'b'
		if tokens[dcbidx] == "dcb.w":
			sizechr = 'w'
		elif tokens[dcbidx] == "dcb.l":
			sizechr = 'l'
		count = eval(tokens[dcbidx+1])
		value = tokens[dcbidx+2]
		line = ""
		for token in tokens[0:dcbidx]:
			line = line + token
		line = line + "\tdc." + sizechr + "\t"
		for i in range(0, count):
			line = line + value
			if i < (count - 1):
				line = line + ", "
		line = line + '\t'
		for token in tokens[dcbidx+3:]:
			line = line + token
	return line

g_rept_mode = False

def rept(line):
	global g_rept_mode
	if ".rept" in line:
		g_rept_mode = True
	if ".endm" in line and g_rept_mode == True:
		g_rept_mode = False
		line = line.replace(".endm", ".endr")
	return line

g_macro_mode = False
g_macro_args = []
g_macro_locals = []
g_macro_name = ""

def macros(line):
	global g_macro_mode
	global g_macro_args
	global g_macro_locals
	global g_macro_name
	if g_macro_mode == False:
		if ".macro" not in line:
			return line
		g_macro_mode = True
		g_macro_locals = []
		g_macro_args = []
		# If handling a macro the name and the .macro need a swap
		tokens = re.split('\t| |,', line)
		if tokens[1] != ".macro":
			return line
		g_macro_name = tokens[0].replace(":", "")
		g_macro_args = tokens[2:]
		g_macro_args.sort(key=len)
		while "" in g_macro_args:
			g_macro_args.remove("")
		line = "\t.macro\t" + g_macro_name
		for arg in g_macro_args:
			line = line + "\t" + arg
		return line
	else:
		if ".endm" in line:
			g_macro_mode = False
			return line
		if ".local" in line:
		#print("Aborting macro subs early due to local in line")
			tokens = re.split('\t| |,', line)
			while "" in tokens:
				tokens.remove("")
			if tokens[0] != ".local":
				return line
			for local in tokens[1:]:
				g_macro_locals.append(local)
			g_macro_locals.sort(key=len)
			return "/*" + line + "*/"
		# Substitute args for local variant
		#print("Canditate arg replacements:")
		#print(g_macro_args)
		for arg in g_macro_args:
			if (arg in line):
				# print("Found arg in line " + arg)
				line = line.replace(arg, '\\' + arg)
		# and local symbols as well
		#print("Canditate local replacements:")
		#print(g_macro_locals)
		for local in g_macro_locals:
			line = line.replace(local, "L"+local+"\\@")
		# Just awful hack to fix the "RV01=0" style checks
		if "\\RV01=0" in line:
			line = line.replace("\\RV01=0", "!\\RV01")
		if "\\RV10=0" in line:
			line = line.replace("\\RV10=0", "!\\RV10")
		return line

def handle_set(line):
	if line[0] == ';':
		return line
	tokens = re.split('\t| |,', line)
	# Remove empty tokens
	tokens[:] = [x for x in tokens if x]
	if (tokens[0] != '.set'):
		return line
	remainder = "";
	for token in tokens[3:]:
		remainder = remainder + token + " "
	return tokens[1] + " = " + tokens[2] + " " + remainder

def handle_equ(line):
	if '\t=\t' not in line:
		return line
	tokens = re.split('\t', line)
	if tokens[1] != '=':
		return line
	return ".set\t" + tokens[0] + "," + tokens[2]

def hex_transform(line):
	# lol
	return line.replace('$', '0x')

def temp_labels(line):
	if "@@:" in line:
		line = line.replace("@@:", "0:", 1)
	if "@F" in line:
		line = line.replace("@F", "0f", 1)
	if "@f" in line:
		line = line.replace("@f", "0f", 1)
	if "@B" in line:
		line = line.replace("@B", "0b", 1)
	if "@b" in line:
		line = line.replace("@b", "0b", 1)
	return line

def binary_exp(line):
	if "#%" in line:
		line = line.replace("#%", "#0b", 1)
		line = line.replace("_0", "0")
		line = line.replace("_1", "1")
	return line

def noneq(line):
	if "<>" in line:
		line = line.replace("<>", "!=", 1)
	return line

def pc_hax(line):
	if ",pc," in line:
		if "\t(" in line:
			line = line.replace('\t(', '\t', 1)
			line = line.replace(',pc,', '(pc,', 1)
	if ".b,pc)" in line:
		if "\t(" in line:
			line = line.replace('\t(', '\t', 1)
			line = line.replace('.b,pc)', '(pc)', 1)
	return line

def include_quotes(line):
	if ".include" in line:
		line = line.replace('.include\t', '.include\t"', 1)
		line = line + '"'
	return line

def comments(line):
	if (line[0] == '*'):
		line = "/* " + line
		line = line + " */"
	elif "\t*" in line or " *" in line:
		line = line.replace("\t*", "\t/*", 1)
		line = line + " */"
	return line

# fake-ass implementation of "dorg" style .offset commands

g_struct_mode = False
g_offset = 0

def offset(line):
	if "\t.offset " not in line:
		return line
	global g_offset
	global g_struct_mode
	line = line.replace("\t.offset ", "", 1)
	g_struct_mode = True
	elements = line.split()
	g_offset = int(elements[0]);
	return ""

def size_for_ds_str(size_str):
	size = 1
	if size_str == "ds.b":
		size = 1
	elif size_str == "ds.w":
		size = 2
	elif size_str == "ds.l":
		size = 4
	return size

def ds(line):
	global g_offset
	global g_struct_mode

	if g_struct_mode == False:
		return line
	if "\tds." not in line:
		return line
	if "STRUCT_SIZE" in line:
		return line
	if "SP_MAX" in line:
		return line

	if "struct_end:" in line:
		g_struct_mode = False
	elements = line.split()
	if ("ds." in elements[0]):
		size = size_for_ds_str(elements[0])
		count = eval(elements[1])
		g_offset = g_offset + (count * size)
		return ""
	else:
		symbol = elements[0].replace(":", "", 1)
		# emit symbol declaration
		line = ".set\t" + symbol + "," + str(g_offset)
		# move Offset based on symbol
		size = size_for_ds_str(elements[1])
		count = eval(elements[2])
		g_offset = g_offset + (count * size)
		return line

def xsp_chart_hax(line):
	if ("XSP_chart_for_512sp_31khz:" in line or
			"XSP_chart_for_512sp_15khz:" in line or
			"struct_top:" in line or
			"struct_end:" in line):
		return line + "\tds.b\t0"
	return line

def parse_all(lines, out_f):
#	comment_on = False;
	for line in lines:
		# print(line)
		line = line.rstrip()
		if len(line) == 0:
			continue
		# TODO: Remove this if typo is fixed. There is a : instead of a * for a
		# comment in the DIV_Y_SUB macro.
		line = line.replace('\t: ', '\t* ')
		line = comments(line)
		line = div_y_sub_hack(line)
		line = pr_protect_hack(line)
		line = rept(line)
		line = macros(line)
		line = include_quotes(line)
		line = noneq(line)
		line = temp_labels(line)
		line = handle_equ(line)
		line = xsp_chart_hax(line)
		line = offset(line)
		line = ds(line)
		line = binary_exp(line)
		line = line.replace('', '')
		line = horrible_xsp_flag_hack(line)
		# GAS isn't smart enough to notice this and thinks you can't do it
		line = line.replace('moveq.l\t#255,', 'moveq.l\t#-1,')
		line = line.replace('moveq.w\t#255,', 'moveq.w\t#-1,')
		line = line.replace('moveq.b\t#255,', 'moveq.b\t#-1,')
		line = line.replace('`', ';', 1)
		line = line.replace('¥', '\\')
		line = hex_transform(line)
		line = parse_dcb(line)
		out_f.write(line)
		out_f.write("\n")

def convert(infname, outfname):
	with open(infname, 'r') as in_f:
		with open(outfname, 'w') as out_f:
			parse_all(in_f.readlines(), out_f)

if __name__ == "__main__":
	if len(sys.argv) < 3:
		print("Usage: ", sys.argv[0], " in out")
	else:
		convert(sys.argv[1], sys.argv[2])
