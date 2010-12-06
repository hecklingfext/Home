//  MyPy.hh for fluxspace.
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
//
// Classes ending in "Ptr" don't change the reference count.
// Classes ending in "Ref" increment and decrement the reference count.

#include <Python.h>
#include <stdio.h>
#include <string>

using namespace std;

class MyPyException
{
  public:
    static void Raise(const string& text_, bool mustShow_ = true);
    static void Print();
    virtual ~MyPyException();

    string text;
    bool   mustShow;

  private:
    MyPyException(const string& text_, bool mustShow_ = true);
};

class MyPyObject
{
  public:
    MyPyObject(bool borrowed_ = false);
    MyPyObject(PyObject* p_, bool borrowed_ = false);
    MyPyObject(const string& s);
    virtual ~MyPyObject();

    void Free();
    operator PyObject*();
    MyPyObject& operator=(PyObject* p_);
    virtual void Assign(PyObject* p_);
    bool IsNull() const { return (p == NULL); }
    bool IsNone() const { return (p == Py_None); }
    MyPyObject GetAttribute(const string& name);

  protected:
    void SetObject(PyObject* p_);

    PyObject* p;
    bool      borrowed;
};

class MyPyObjectPtr : public MyPyObject
{
  public:
    MyPyObjectPtr();
    MyPyObjectPtr(PyObject* p_);
};

class MyPyObjectRef : public MyPyObject
{
  public:
    MyPyObjectRef();
    MyPyObjectRef(PyObject* p_);
    virtual void Assign(PyObject* p_);
};

class MyPyDictPtr : public MyPyObjectPtr
{
  public:
    MyPyDictPtr();
    MyPyDictPtr(PyObject* object);
    MyPyDictPtr& operator=(PyObject* object);

    MyPyObjectPtr GetItemString(const string& key);
};

class MyPyModule : public MyPyObject
{
  public:
    MyPyModule(const string& name);

    MyPyDictPtr dict;

  protected:
    void Import(const string& name);
};

class MyPyListPtr : public MyPyObjectPtr
{
  public:
    MyPyListPtr();
    MyPyListPtr(PyObject* object);
    MyPyListPtr& operator=(PyObject* object);

    void InsertString(const string& s, int index = 0);
    void AppendString(const string& s);
};

class MyPyFunctionPtr : public MyPyObjectPtr
{
  public:
    MyPyFunctionPtr(bool optional_ = false);
    MyPyFunctionPtr(PyObject* object, bool optional_ = false);
    virtual ~MyPyFunctionPtr();

    MyPyFunctionPtr& operator=(const MyPyFunctionPtr& src);

    virtual void BeginArgs(int n);
    virtual void AddArg(PyObject* arg);
    virtual void AddArgInt(long i);
    virtual void AddArgString(const string& s);
    virtual void AddArgObject(PyObject* arg);   // INCREF'd

    virtual void Call();

    bool       optional;
    MyPyObject args;
    int        nArgs;

  protected:
    bool initArgs;

  private:
    MyPyFunctionPtr& operator=(PyObject* object);
};

class MyPyMethod : public MyPyObjectRef
{
  public:
    MyPyMethod(PyObject* object, const string& name_, bool optional_ = false);
    virtual ~MyPyMethod();

    MyPyMethod& operator=(const MyPyMethod& src);

    virtual void BeginArgs(int n);
    virtual void AddArg(PyObject* arg);
    virtual void AddArgInt(long i);
    virtual void AddArgString(const string& s);
    virtual void AddArgObject(PyObject* arg);   // INCREF'd
    virtual void Call();

    string     name;
    bool       optional;
    MyPyObject args;
    int        nArgs;

  protected:
    bool initArgs;

  private:
    MyPyMethod(bool optional_ = false);
    MyPyObjectRef& operator=(PyObject* object);
};

class MyPython
{
  public:
    MyPython();
    virtual ~MyPython();

	void InsertModuleDirectory(const string& dir);
	void AppendModuleDirectory(const string& dir);
};
