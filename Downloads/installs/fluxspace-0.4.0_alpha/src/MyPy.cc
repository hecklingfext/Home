//  MyPy.cc for fluxspace.
//
//  Copyright (c) 2002 Steve Cooper, stevencooper@isomedia.com
//  Copyright (c) 1998-2000 by John Kennis, jkennis@chello.nl
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//

#include "MyPy.hh"

void MyPyException::Raise(const string& text_, bool mustShow_)
{
    PyErr_Print();      // Just in case something useful will be displayed
    throw MyPyException(text_, mustShow_);
}

void MyPyException::Print()
{
    if (PyErr_Occurred() != NULL)
        PyErr_Print();
}

MyPyException::MyPyException(const string& text_, bool mustShow_)
  : text(text_),
    mustShow(mustShow_)
{
}

MyPyException::~MyPyException()
{
    if (PyErr_Occurred() != NULL)
    {
        if (mustShow)
            PyErr_Print();
        else
            PyErr_Clear();
    }
}

MyPyObject::MyPyObject(bool borrowed_)
  : p(NULL),
    borrowed(borrowed_)
{
}

MyPyObject::MyPyObject(PyObject* p_, bool borrowed_)
  : p(NULL),
    borrowed(borrowed_)
{
    Assign(p_);
}

MyPyObject::MyPyObject(const string& s)
  : p(NULL),
    borrowed(false)
{
    Assign(PyString_FromString(s.c_str()));
}

MyPyObject::~MyPyObject()
{
    Free();
}

void MyPyObject::Free()
{
    if (p)
    {
        if (!borrowed)
            Py_DECREF(p);
        p = NULL;
    }
}

MyPyObject::operator PyObject*()
{
    return p;
}

MyPyObject& MyPyObject::operator=(PyObject* p_)
{
    Assign(p_);
    return *this;
}

void MyPyObject::Assign(PyObject* p_)
{
    Free();
    p = p_;
}

void MyPyObject::SetObject(PyObject* p_)
{
    p = p_;
}

MyPyObject MyPyObject::GetAttribute(const string& name)
{
    char* aname = const_cast<char*>(name.c_str());
    PyObject* pObj = NULL;
    if (p != NULL && PyObject_HasAttrString(p, aname))
        pObj = PyObject_GetAttrString(p, aname);
    return pObj;
}

MyPyObjectPtr::MyPyObjectPtr()
  : MyPyObject(true)
{
}

MyPyObjectPtr::MyPyObjectPtr(PyObject* p_)
  : MyPyObject(p_, true)
{
}

MyPyObjectRef::MyPyObjectRef()
  : MyPyObject(false)
{
}

MyPyObjectRef::MyPyObjectRef(PyObject* p_)
  : MyPyObject(p_, false)
{
    Py_INCREF(p);
}

void MyPyObjectRef::Assign(PyObject* p_)
{
    MyPyObject::Assign(p_);
    Py_INCREF(p);
}

MyPyListPtr::MyPyListPtr()
{
}

MyPyListPtr::MyPyListPtr(PyObject* object)
  : MyPyObjectPtr(object)
{
}

MyPyListPtr& MyPyListPtr::operator=(PyObject* object)
{
    Assign(object);
    return *this;
}

void MyPyListPtr::InsertString(const string& s, int index)
{
    PyList_Insert(p, index, MyPyObject(s));
}

void MyPyListPtr::AppendString(const string& s)
{
    PyList_Append(p, MyPyObject(s));
}

MyPyDictPtr::MyPyDictPtr()
{
}

MyPyDictPtr::MyPyDictPtr(PyObject* object)
  : MyPyObjectPtr(object)
{
}

MyPyDictPtr& MyPyDictPtr::operator=(PyObject* object)
{
    Assign(object);
    return *this;
}

MyPyObjectPtr MyPyDictPtr::GetItemString(const string& key)
{
    MyPyObjectPtr object;
    try
    {
        object = PyDict_GetItemString(*this, const_cast<char*>(key.c_str()));
    }
    catch(MyPyException e)
    {
        e.mustShow = false;
        object = Py_None;
    }
    return object;
}

MyPyFunctionPtr::MyPyFunctionPtr(bool optional_)
  : optional(optional_),
    nArgs(0),
    initArgs(false)
{
}

MyPyFunctionPtr::MyPyFunctionPtr(PyObject* object, bool optional_)
  : MyPyObjectPtr(object),
    optional(optional_),
    nArgs(0),
    initArgs(false)
{
}

MyPyFunctionPtr::~MyPyFunctionPtr()
{
}

MyPyFunctionPtr& MyPyFunctionPtr::operator=(const MyPyFunctionPtr& src)
{
    Assign(src.p);
    optional = src.optional;
    return *this;
}

void MyPyFunctionPtr::BeginArgs(int n)
{
    args.Assign(PyTuple_New(n));
    nArgs = 0;
    initArgs = true;
}

void MyPyFunctionPtr::AddArg(PyObject* arg)
{
    if (args.IsNull())
        MyPyException::Raise("No arguments prepared by BeginArgs()");
    if (PyTuple_Size(args) < nArgs+1)
        MyPyException::Raise("Too many arguments to function");
    if (PyTuple_SetItem(args, nArgs, arg) != 0)
        MyPyException::Raise("Failed to set argument");
    nArgs++;
}

void MyPyFunctionPtr::AddArgInt(long i)
{
    AddArg(PyInt_FromLong(i));
}

void MyPyFunctionPtr::AddArgString(const string& s)
{
    AddArg(PyString_FromString(s.c_str()));
}

void MyPyFunctionPtr::AddArgObject(PyObject* pObj)
{
    AddArg(pObj);
    Py_INCREF(pObj);
}

void MyPyFunctionPtr::Call()
{
    bool failed = (   IsNull()
                   || IsNone()
                   || (PyObject_CallObject(p, args) == NULL && !optional));
    args.Free();
    nArgs = 0;
    initArgs = false;
    if (failed)
    {
        PyErr_Print();
        MyPyException::Raise("Function call failed");
    }
}

MyPyMethod::MyPyMethod(PyObject* object, const string& name_, bool optional_)
  : MyPyObjectRef(object),
    name(name_),
    optional(optional_),
    nArgs(0),
    initArgs(false)
{
}

MyPyMethod::~MyPyMethod()
{
}

MyPyMethod& MyPyMethod::operator=(const MyPyMethod& src)
{
    Assign(const_cast<MyPyMethod&>(src));
    name = src.name;
    optional = src.optional;
    return *this;
}

void MyPyMethod::BeginArgs(int n)
{
    args.Assign(PyTuple_New(n+1));
    nArgs = 0;
    AddArgObject(p);
    initArgs = true;
}

void MyPyMethod::AddArg(PyObject* arg)
{
    if (args.IsNull())
        MyPyException::Raise("No arguments prepared by BeginArgs()");
    if (PyTuple_Size(args) < nArgs+1)
        MyPyException::Raise("Too many arguments to function");
    if (PyTuple_SetItem(args, nArgs, arg) != 0)
        MyPyException::Raise("Failed to set argument");
    nArgs++;
}

void MyPyMethod::AddArgInt(long i)
{
    AddArg(PyInt_FromLong(i));
}

void MyPyMethod::AddArgString(const string& s)
{
    AddArg(PyString_FromString(s.c_str()));
}

void MyPyMethod::AddArgObject(PyObject* pObj)
{
    AddArg(pObj);
    Py_INCREF(pObj);
}

void MyPyMethod::Call()
{
    bool succeeded = false;

    if (!IsNull() && !IsNone())
    {
        try
        {
            MyPyObject methObject = GetAttribute(name.c_str());
            if (!methObject.IsNull() && !methObject.IsNone())
            {
                MyPyObjectPtr funcObject = PyMethod_Function(methObject);
                if (!funcObject.IsNull() && !funcObject.IsNone())
                {
                    // Make sure self is there
                    if (!initArgs)
                        BeginArgs(0);

                    if (PyObject_CallObject(funcObject, args) != NULL)
                        succeeded = true;
                }
            }
        }
        catch(MyPyException e)
        {
            // Automatically printed by the exception object
        }

        args.Free();
        nArgs = 0;
        initArgs = false;
    }

    if (!succeeded)
    {
        PyErr_Print();
        if (!optional || PyErr_Occurred() != NULL)
        {
            string msg = "Method call to \"";
            msg += name;
            msg += "\" failed";
            PyErr_Clear();
            MyPyException::Raise(msg);
        }
    }
}

MyPyModule::MyPyModule(const string& name)
{
    Import(name);
    dict = PyModule_GetDict(*this);
}

void MyPyModule::Import(const string& name)
{
    if (name.length() == 0)
        MyPyException::Raise("NULL module name");
    PyObject* module = PyImport_Import(MyPyObject(name));
    if (module == NULL || PyErr_Occurred() != NULL)
    {
        string s = "Failed to load module \"";
        s += name;
        s += "\"";
        //PyErr_Clear();
        MyPyException::Raise(s);
    }
    Assign(module);
}

MyPython::MyPython()
{
    Py_Initialize();
}

MyPython::~MyPython()
{
    Py_Finalize();
}

void MyPython::InsertModuleDirectory(const string& dir)
{
    MyPyModule sys("sys");
    MyPyObjectPtr path = sys.dict.GetItemString("path");
    MyPyListPtr(path).InsertString(dir);
}

void MyPython::AppendModuleDirectory(const string& dir)
{
    MyPyModule sys("sys");
    MyPyObjectPtr path = sys.dict.GetItemString("path");
    MyPyListPtr(path).AppendString(dir);
}
