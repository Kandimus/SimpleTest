# This Python file uses the following encoding: utf-8

# if__name__ == "__main__":
#     pass
import re
import sys
import argparse


templateFile = '// Autogenerated!!!\n' + \
					'// don\'t modify this file!!!\n' + \
					'\n' + \
					'#include "simpletest.h"\n' + \
					'\n' + \
					'{FUNCS_DECL}\n\n' + \
					'rSimpleTest::rSimpleTest(void)\n' + \
					'{{\n' + \
					'{FUNCS_ADD}' + \
					'}}\n'

class TestFunc:
	def __init__(self, name, descr):
		self.name  = name
		self.descr = descr

	def toFile(self, prefix):
		return prefix + 'm_tests.push_back(new rItemType("{NAME}", {DESCR}, &SimpleTest_{NAME}));\n'.format(NAME=self.name, DESCR=self.descr)

	def toDecl(self, prefix):
		return prefix + 'void SimpleTest_{NAME}(void);\n'.format(NAME=self.name)


gTests = []


def readFile(filename, list):
	with open(filename, 'r') as f:
		for line in f:
			newtests = re.findall(r'\s*S_NEW_TEST\s*\(\s*[a-zA-Z]+[0-9a-zA-Z_]*\s*, .*\)', f.read())
			for test in newtests:
				args      = re.split(r'\(\s*', test, 1)
				args      = args[1].rsplit(')', 1)
				splitargs = re.split(r'\s*,\s*', args[0], 1)
				nametest  = splitargs[0]
				descrtest = splitargs[1]
				list.append(TestFunc(splitargs[0], splitargs[1]))


def createParser ():
	parser = argparse.ArgumentParser()
	parser.add_argument ('-f', '--files' , nargs='+')
	parser.add_argument ('-d', '--dir'   , default=['./'])
	parser.add_argument ('-o', '--output', default=['./simpletest.gen.cpp'])
	return parser


if __name__ == '__main__':
	parser    = createParser()
	namespace = parser.parse_args(sys.argv[1:])

	if namespace.dir[-1] != '//':
		namespace.dir += '//'

	for file in namespace.files:
		readFile(namespace.dir + file, gTests)

	outFile = open(namespace.output, 'w')

	textDecl = '';
	textAdd  = '';

	for t in gTests:
		textDecl += t.toDecl('')
		textAdd  += t.toFile('\t')

	outFile.write(templateFile.format(FUNCS_DECL=textDecl, FUNCS_ADD=textAdd))
	outFile.close()
