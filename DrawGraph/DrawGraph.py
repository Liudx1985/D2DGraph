# -*-  encoding=UTF-8 -*-

from io import StringIO
import sys

old_stderr = sys.stderr
old_stdout = sys.stdout
mystdout = StringIO();



# blah blah lots of code ...
def redirectStdIO():
	global mystdout
	sys.stdout = mystdout
	sys.stderr = mystdout
	print("init succeed.")


def getStdIOContent():
	global mystdout;
	cont = mystdout.getvalue()
	mystdout.seek(0);
	mystdout.truncate(0); # clear all.
	return cont;

def resetStdIO():
	global old_stdout,old_stderr,mystdout;
	sys.stdout = old_stdout
	sys.stderr = old_stderr
	print("Uninit succeed.")
	mystdout.close()

def test():
	redirectStdIO();
	print("fuck me god");
	cont = getStdIOContent();
	print("fuck me girl")
	cont2 = getStdIOContent();

	resetStdIO();
	print (cont, end='')
	print (cont2, end='')

#test();
from direct2d import *