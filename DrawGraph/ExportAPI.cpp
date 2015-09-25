#include "stdafx.h"
#include "ExportAPI.h"
// extra using link
#include <fstream>
#include "DrawGraph.h"
#include "DrawGraphDoc.h"
#include "MainFrm.h"
#include "DrawGraphView.h"

BOOST_PYTHON_MODULE(direct2d) //generate a PyInit_direct2d function for init this module.
{
	namespace python = boost::python;
	
	python::class_<D2D1_POINT_2F>("Point2F")
		.def(init<>())
		.def_readwrite("x", (&D2D1_POINT_2F::x))
		.def_readwrite("y", (&D2D1_POINT_2F::y))
		;
	python::class_<D2D1_VECTOR_2F>("Vector2F")
		.def(init<>())
		.def_readwrite("x", (&D2D1_VECTOR_2F::x))
		.def_readwrite("y", (&D2D1_VECTOR_2F::y))
		;

	def("ClearCanvas", &ClearCanvas);
	def("SetBrushColor", &SetBrushColor);
	def("DrawLine", &DrawLine);
	def("DrawCircle", DrawCircle);
	def("DrawRectangle", DrawRectangle);
}

bool InitPyEnv()
{
	int iRet = PyImport_AppendInittab("direct2d", &PyInit_direct2d);
	Py_Initialize(); // to start the interpreter and create the _main_ module. 
	bool bResult = false;
	try
	{
		//	init_module_direct2d();
		object _main_ = import("__main__");
		object global(_main_.attr("__dict__"));

		std::string strFileName;
		strFileName.resize(512);
		GetModuleFileName(theApp.m_hInstance, &strFileName.front(), strFileName.size());
		size_t tPos = strFileName.find_last_of(".");
		strFileName.resize(tPos + 1);		
		strFileName += "py";

		object ignored = exec_file(strFileName.c_str(), global, global);
		ignored = exec("redirectStdIO()", global, global);
		ignored = exec("print('Python embed in c++(Powered by liudx@1985gmail.com)');",
			global, global);

		object fun_get = global["getStdIOContent"];
		object t = fun_get();
		theApp.AddDebugLog(extract<char *>(t), 0xff00);
		bResult = true;
	}
	catch (error_already_set&)
	{
		PyErr_Print();
	}
	return bResult;
}
//Quit & Clear
void UnInitPyEnv()
{
	try
	{
		object _main_ = import("__main__");
		object global(_main_.attr("__dict__"));
		object ignored = exec("resetStdIO()", global, global);
	}
	catch (error_already_set &e)
	{
		PyErr_Print();
		//std::cout << e << std::endl;	
	}
	Py_Finalize();
}
// called by host. not script.
bool EvalPyScript(std::string const &strScript)
{
	bool bResult = false;
	if (strScript.empty())
	{
		return bResult;
	}
	CString strMsg = ">";
	strMsg += strScript.c_str();
	strMsg += "\n";
	theApp.AddDebugLog(LPCTSTR(strMsg), 0);

	object _main_ = import("__main__");
	object global(_main_.attr("__dict__"));
	try
	{		
		object ignored = exec(strScript.c_str(), global, global);
		bResult = true;
	}
	catch (error_already_set &e)
	{
		PyErr_Print();
		//std::cout << e << std::endl;	
	}
	object fun_get = global["getStdIOContent"];
	object t = fun_get();
	theApp.AddDebugLog(extract<char *>(t), 0xff);
	return bResult;
}

bool EvalPyScriptFile(std::string const &strFile)

{
	bool bResult = false;
	if (strFile.empty())
	{
		return bResult;
	}
	CString strMsg = "[exec file]";
	strMsg += strFile.c_str();
	strMsg += "\n";
	theApp.AddDebugLog(LPCTSTR(strMsg), 0xFF00);

	object _main_ = import("__main__");
	object global(_main_.attr("__dict__"));
	try
	{
		object ignored = exec_file(strFile.c_str(), global, global);
		bResult = true;
	}
	catch (error_already_set &e)
	{
		PyErr_Print();
		//std::cout << e << std::endl;	
	}
	object fun_get = global["getStdIOContent"];
	object t = fun_get();
	theApp.AddDebugLog(extract<char *>(t), 0xff);
	return bResult;
}

static CDrawGraphView *GetGrahpView()
{
	static CDrawGraphView *s_pView = NULL;
	if (NULL == s_pView)
	{
		s_pView = theApp.GetGraphView();
	}
	return s_pView;
}

void ClearCanvas()
{
	CDrawGraphView *pView = GetGrahpView();
	if (pView)
	{
		pView->ClearCanvas();
	}
}

_declspec(dllexport) void SetBrushColor(int iColor)
{
	CDrawGraphView *pView = GetGrahpView();
	if (pView)
	{
		pView->SetBrushColor(iColor);
	}
}

//
void DrawLine(D2D1_POINT_2F const &a, D2D1_VECTOR_2F const &b)
{
	CDrawGraphView *pView = GetGrahpView();
	if (pView)
	{
		pView->DrawLine(a, b);
	}
}


void DrawLine(float x, float y, float xDir, float yDir)
{
	D2D1_POINT_2F a = { x, y };
	D2D1_VECTOR_2F v = { xDir, yDir };
	DrawLine(a, v);
}
//
void DrawRectangle(float x, float y, float wid, float hei)
{
	CDrawGraphView *pView = GetGrahpView();
	if (pView)
	{
		D2D1_RECT_F rc = { x, y, x + wid, y + hei };
		pView->DrawRectangle(rc);
	}
}
//
void DrawCircle(float x, float y, float radius)
{
	CDrawGraphView *pView = GetGrahpView();
	if (pView)
	{
		D2D1_POINT_2F c = { x, y };
		D2D1_ELLIPSE eps = {c, radius, radius };
		pView->DrawELLIPSE(eps);
	}
}