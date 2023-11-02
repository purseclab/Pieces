import clang.cindex
import sys
import re
import utils
import os
from pathlib import Path

# A simple visitor for FuncDef nodes that prints the names and
# locations of function definitions.
def remove_comments(source_code):
	# Remove single-line comments (//) and multi-line comments (/* ... */)
	# This regex handles both single-line and multi-line comments.
	pattern = r'(/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*)'
	return re.sub(pattern, '', source_code)

def remove_includes(source_code):
	# Remove all #include directives
	pattern = r'#include\s+["<].*?[>"]'
	return re.sub(pattern, '', source_code)

def remove_directives(source_code):
	# Remove all preprocessor directives
	pattern = r'#\s*(include|define|ifdef|ifndef|else|elif|endif|pragma|warning|error|line|undef|if|elif|else|endif|defined)\s*.*'
	return re.sub(pattern, '', source_code)

def remove_gcc_attributes(source_code):
	# Remove GCC-style attributes (__attribute__((...)))
	pattern = r'__attribute__\s*\(\([^)]*\)\)'
	return re.sub(pattern, '', source_code)

def print_ast(cursor, depth =0):
	print ('  '* depth, str(cursor.extent.start.line) + ":"  +str( cursor.kind) + "::" +str(cursor.spelling))
	for child in cursor.get_children():
			print_ast(child, depth + 1)
def print_pragma_directives(cursor, depth=0):
	print (str(cursor.extent.start.line) + ":"  +str( cursor.kind) + "::" +str(cursor.spelling))
	if cursor.kind == clang.cindex.CursorKind.PREPROCESSING_DIRECTIVE:
		print(cursor.displayname)
		# Check if the cursor represents a pragma directive
		if cursor.displayname.startswith("#pragma"):
			# Print the pragma directive
			print('  ' * depth, cursor.displayname)
	for child in cursor.get_children():
		print_pragma_directives(child, depth + 1)


def get_function_attributes(cursor):
	attributes = []
	for attr in cursor.get_children():
		if attr.kind == clang.cindex.CursorKind.ANNOTATE_ATTR:
			attributes.append(attr.spelling)
	return attributes

def print_cursor_kind(cursor, depth=0):
	print ('  '* depth, str(cursor.location.file) + ":" + str(cursor.location.line)+ ":"  +str( cursor.kind) + "::" +str(cursor.spelling))

def print_token(token):
	print(str(token.location.file)+ ":" +str(token.location.line))
	print(token.cursor.spelling)

def print_function_prototypes(cursor, depth=0):
	if cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL:
		# Check if the cursor represents a function definition
		if cursor.is_definition():
			# Print the complete function prototype
			function_name = cursor.spelling
			function_type = cursor.type.spelling


			# Retrieve and print the function parameters
			param_list = []
			for param in cursor.get_children():
				if param.kind == clang.cindex.CursorKind.PARM_DECL:
					param_list.append(param.type.spelling)
			parameters = ', '.join(param_list)

			# Retrieve and print function attributes
			attributes = get_function_attributes(cursor)
			attributes_str = ' '.join(attributes)

			print('  ' * depth, function_type, attributes_str, function_name, f'({parameters})')

	for child in cursor.get_children():
		print_function_prototypes(child, depth + 1)
	

class Pragma():
	def __init__(self, file, line, str, tokens):
				self.file = file
				self.line = line
				self.str = str
				self.tokens = tokens

	def __str__(self):
			return "Pragma of type:" + str(self.tokens)

def parse_pragmas(filename):
	pragmas = []
	f = open(filename)
	linenum =0
	for line in f:
			linenum += 1
			matches = re.findall('#pragma\s+[^\r\n]+', line)
			if matches:
				line = matches[0]
				tokens = line.split(" ")
				pragma = Pragma(filename, linenum, line, tokens[1:])
				pragmas.append(pragma)
	f.close()
	return pragmas
	
def parse_c_program(filename):
	# Create an index for parsing
#	clang.cindex.Config.set_library_path("/home/arslan/projects/LBC/checkedC-12/checkedc-clang/buildmk/lib")
	clang.cindex.Config.set_library_path(os.environ["LIBCLANG"])
	index = clang.cindex.Index.create()

	# Parse the C program
	translation_unit = index.parse(filename)

	# Traverse the AST and extract information
	def traverse(node, depth=0):
		if (node.kind == clang.cindex.CursorKind.FUNCTION_DECL):
			print("  " * depth + node.displayname)
			print(dir(node.type))
		for child in node.get_children():
			traverse(child, depth + 1)

	# Start traversing the AST from the root cursor
	root_cursor = translation_unit.cursor

	return translation_unit

def preprocess(filename, config_options_string, project_root):
	args = "clang-cpp "+ config_options_string + " " + filename
	cpp_args = args.split(" ")
	path = Path(filename)
	f = open(os.environ["P_OUT_DIR"] + "pp_" + path.name, "w+")
	utils.run_cmd(cpp_args, out=f, cwd_arg=project_root)
	f.seek(0)
	f.close()
	return f
	
#	traverse(root_cursor)
#	print_function_prototypes(root_cursor)
#	print_pragma_directives(root_cursor)
#	print_ast(root_cursor)

def find_functions(cursor):
	# Check if the cursor represents a function
	if cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL:
		print(f"Function: {cursor.spelling}")

	# Recursively traverse the children of the cursor
	for child in cursor.get_children():
		find_functions(child)

def find_function_names(cursor):
	function_names = []

	if cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL:
		function_names.append(cursor.spelling)

	for child in cursor.get_children():
		function_names.extend(find_function_names(child))

	return function_names

def find_function_names_in_subset(cursor_set):
	function_names = []

	for cursor in cursor_set:
		if cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL:
			function_names.append(cursor.spelling)
	return function_names

def get_tokens_in_range(file_path, start_line, start_column, end_line, end_column):
	index = clang.cindex.Index.create()
	try:
		translation_unit = index.parse(file_path)
	except clang.cindex.TranslationUnitLoadError as e:
		print(f"Error parsing translation unit: {e}")
		error_message = e.args[0]  # Get the error message
		error_details = e.args[1] if len(e.args) > 1 else None  # Get additional error details

		print(f"Error message: {error_message}")
		if error_details is not None:
			print(f"Error details: {error_details}")
		return
	except FileNotFoundError:
		print("File not found.")
		return
	except Exception as e:
		print(f"An error occurred: {e}")
		return

	start_location = translation_unit.get_location(file_path, [start_line, start_column])
	end_location = translation_unit.get_location(file_path, [end_line, end_column])

	# Retrieve the translation unit cursor
	tu_cursor = translation_unit.cursor

	# Find the token range within the specified source code range
	tokens = []
	for token in tu_cursor.get_tokens():
		token_location = token.location
		if (token_location.line, token_location.column) >= (start_line, start_column) and \
		   (token_location.line, token_location.column) <= (end_line, end_column):
			tokens.append(token)

	return tokens


def get_cursor(file_path):
	index = clang.cindex.Index.create()
	translation_unit = index.parse(file_path)
	return translation_unit.cursor
	
def get_cursors_in_range(icursor, start_line, start_column, end_line, end_column, ocursors):
	if start_line <= icursor.location.line <= end_line and start_column <= icursor.location.column <= end_column:
#print_cursor_kind(icursor)
		ocursors.append(icursor)
	for node in icursor.get_children():
		# Recursively check child nodes
		get_cursors_in_range(node, start_line, start_column, end_line, end_column, ocursors)

def find_casts(cursor):
	if cursor.kind == clang.cindex.CursorKind.CSTYLE_CAST_EXPR:
		print(f"Type cast: {cursor.spelling}")

def find_casts_in_subset(cursor_set):
	casts = []

	for cursor in cursor_set:
		if cursor.kind == clang.cindex.CursorKind.CSTYLE_CAST_EXPR:
			print(str(cursor.location.file) + ":" + str(cursor.location.line))
			print(cursor.spelling)
			casts.append(cursor.spelling)
	return casts
