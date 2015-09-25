#pragma once


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <complex>

#include <boost/python.hpp>
using namespace std;
#include "python.h"
#include <boost/any.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include "array_indexing_suite.hpp"

using namespace boost::python;

// clear the canvas
extern "C"
{
	bool InitPyEnv(); // call this before you eval script
	void UnInitPyEnv();
	// Eval script.
	bool EvalPyScript(std::string const &strScript);
	bool EvalPyScriptFile(std::string const &strFile);
	_declspec(dllexport) void ClearCanvas(void);
	_declspec(dllexport) void SetBrushColor(int iColor);

	// draw a line from a, director vector is v
	_declspec(dllexport) void DrawLine(float x, float y, float xDir, float yDir);
	_declspec(dllexport) void DrawRectangle(float x, float y, float wid, float hei);
	_declspec(dllexport) void DrawCircle(float x, float y, float radius);
}
