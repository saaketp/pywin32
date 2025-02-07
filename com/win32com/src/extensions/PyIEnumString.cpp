// This file implements the IEnumString Interface and Gateway for Python.
// Generated by makegw.py

#include "stdafx.h"
#include "PythonCOM.h"
#include "PythonCOMServer.h"
#include "PyIEnumString.h"

// @doc - This file contains autoduck documentation

// ---------------------------------------------------
//
// Interface Implementation

PyIEnumString::PyIEnumString(IUnknown *pdisp) : PyIUnknown(pdisp) { ob_type = &type; }

PyIEnumString::~PyIEnumString() {}

/* static */ IEnumString *PyIEnumString::GetI(PyObject *self) { return (IEnumString *)PyIUnknown::GetI(self); }

// @pymethod (<o PyUnicode>,...)|PyIEnumString|Next|Retrieves a specified number of items in the enumeration sequence.
PyObject *PyIEnumString::Next(PyObject *self, PyObject *args)
{
    ULONG celt = 1;
    // @pyparm int|num|1|Number of items to retrieve.
    if (!PyArg_ParseTuple(args, "|I:Next", &celt))
        return NULL;

    IEnumString *pIES = GetI(self);
    if (pIES == NULL)
        return NULL;

    LPOLESTR *rgVar = new LPOLESTR[celt];
    if (rgVar == NULL)
        return PyErr_Format(PyExc_MemoryError, "Unable to allocate %d string pointers", celt);
    ZeroMemory(rgVar, celt * sizeof(LPOLESTR));

    ULONG celtFetched, i;
    PyObject *result;

    PY_INTERFACE_PRECALL;
    HRESULT hr = pIES->Next(celt, rgVar, &celtFetched);
    PY_INTERFACE_POSTCALL;
    if (FAILED(hr))
        result = PyCom_BuildPyException(hr);
    else {
        result = PyTuple_New(celtFetched);
        if (result != NULL) {
            for (i = celtFetched; i--;) {
                PyObject *ob = PyWinObject_FromWCHAR(rgVar[i]);
                if (ob == NULL) {
                    Py_DECREF(result);
                    result = NULL;
                    break;
                }
                PyTuple_SET_ITEM(result, i, ob);
            }
        }
    }

    for (i = 0; i < celt; i++)
        if (rgVar[i] != NULL)
            CoTaskMemFree(rgVar[i]);
    delete[] rgVar;
    return result;
}

// @pymethod |PyIEnumString|Skip|Skips over the next specified elementes.
PyObject *PyIEnumString::Skip(PyObject *self, PyObject *args)
{
    ULONG celt;
    if (!PyArg_ParseTuple(args, "k:Skip", &celt))
        return NULL;

    IEnumString *pIES = GetI(self);
    if (pIES == NULL)
        return NULL;

    PY_INTERFACE_PRECALL;
    HRESULT hr = pIES->Skip(celt);
    PY_INTERFACE_POSTCALL;
    if (FAILED(hr))
        return PyCom_BuildPyException(hr);

    Py_INCREF(Py_None);
    return Py_None;
}

// @pymethod |PyIEnumString|Reset|Resets the enumeration sequence to the beginning.
PyObject *PyIEnumString::Reset(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":Reset"))
        return NULL;

    IEnumString *pIES = GetI(self);
    if (pIES == NULL)
        return NULL;

    PY_INTERFACE_PRECALL;
    HRESULT hr = pIES->Reset();
    PY_INTERFACE_POSTCALL;
    if (FAILED(hr))
        return PyCom_BuildPyException(hr);

    Py_INCREF(Py_None);
    return Py_None;
}

// @pymethod <o PyIEnumString>|PyIEnumString|Clone|Creates another enumerator that contains the same enumeration state
// as the current one
PyObject *PyIEnumString::Clone(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":Clone"))
        return NULL;

    IEnumString *pIES = GetI(self);
    if (pIES == NULL)
        return NULL;

    IEnumString *pClone;
    PY_INTERFACE_PRECALL;
    HRESULT hr = pIES->Clone(&pClone);
    PY_INTERFACE_POSTCALL;
    if (FAILED(hr))
        return PyCom_BuildPyException(hr);

    return PyCom_PyObjectFromIUnknown(pClone, IID_IEnumString, FALSE);
}

// @object PyIEnumString|An enumerator interface to list strings
static struct PyMethodDef PyIEnumString_methods[] = {
    {"Next", PyIEnumString::Next,
     METH_VARARGS},  // @pymeth Next|Retrieves a specified number of items in the enumeration sequence.
    {"Skip", PyIEnumString::Skip, METH_VARARGS},    // @pymeth Skip|Skips over the next specified elementes.
    {"Reset", PyIEnumString::Reset, METH_VARARGS},  // @pymeth Reset|Resets the enumeration sequence to the beginning.
    {"Clone", PyIEnumString::Clone, METH_VARARGS},  // @pymeth Clone|Creates another enumerator that contains the same
                                                    // enumeration state as the current one.
    {NULL}};

PyComEnumTypeObject PyIEnumString::type("PyIEnumString",
                                        &PyIUnknown::type,  // @base PyIEnumString|PyIUnknown
                                        sizeof(PyIEnumString), PyIEnumString_methods, GET_PYCOM_CTOR(PyIEnumString));

// ---------------------------------------------------
//
// Gateway Implementation

STDMETHODIMP PyGEnumString::Next(ULONG celt, LPOLESTR __RPC_FAR *rgVar, ULONG __RPC_FAR *pCeltFetched)
{
    PY_GATEWAY_METHOD;
    PyObject *result, *result_tuple, *result_item;
    ULONG item_index;
    HRESULT hr = InvokeViaPolicy("Next", &result, "i", celt);
    if (FAILED(hr))
        return hr;

    // Caller is expected to allocate array of string pointers, server allocates strings themselves
    ZeroMemory(rgVar, celt * sizeof(LPOLESTR));
    result_tuple = PySequence_Tuple(result);
    if (result_tuple == NULL)
        return PyCom_SetCOMErrorFromPyException(IID_IEnumString);
    hr = S_OK;
    *pCeltFetched = PyTuple_GET_SIZE(result_tuple);
    if (*pCeltFetched > celt) {
        PyErr_Format(PyExc_ValueError, "Received %d items , but only %d items requested", *pCeltFetched, celt);
        hr = PyCom_SetCOMErrorFromPyException(IID_IEnumString);
    }
    else
        for (item_index = 0; item_index < *pCeltFetched; item_index++) {
            result_item = PyTuple_GET_ITEM(result_tuple, item_index);
            if (!PyWinObject_AsTaskAllocatedWCHAR(result_item, &rgVar[item_index], FALSE)) {
                hr = PyCom_SetCOMErrorFromPyException(IID_IEnumString);
                break;
            }
        }

    Py_DECREF(result_tuple);
    if (hr != S_OK)
        for (item_index = 0; item_index < *pCeltFetched; item_index++)
            if (rgVar[item_index] != NULL) {
                PyWinObject_FreeTaskAllocatedWCHAR(rgVar[item_index]);
                rgVar[item_index] = NULL;
            }
    return hr;
}

STDMETHODIMP PyGEnumString::Skip(ULONG celt)
{
    PY_GATEWAY_METHOD;
    return InvokeViaPolicy("Skip", NULL, "i", celt);
}

STDMETHODIMP PyGEnumString::Reset(void)
{
    PY_GATEWAY_METHOD;
    return InvokeViaPolicy("Reset");
}

STDMETHODIMP PyGEnumString::Clone(IEnumString __RPC_FAR *__RPC_FAR *ppEnum)
{
    PY_GATEWAY_METHOD;
    PyObject *result;
    HRESULT hr = InvokeViaPolicy("Clone", &result);
    if (FAILED(hr))
        return hr;

    if (!PyIBase::is_object(result, &PyIUnknown::type)) {
        // the wrong kind of object was returned to us
        Py_DECREF(result);
        return PyCom_SetCOMErrorFromSimple(E_FAIL, IID_IEnumString);
    }

    IUnknown *punk = ((PyIUnknown *)result)->m_obj;
    if (!punk) {
        Py_DECREF(result);
        return PyCom_SetCOMErrorFromSimple(E_FAIL, IID_IEnumString);
    }

    Py_BEGIN_ALLOW_THREADS hr = punk->QueryInterface(IID_IEnumString, (LPVOID *)ppEnum);
    Py_END_ALLOW_THREADS

        // done with the result; this DECREF is also for <punk>
        Py_DECREF(result);

    return PyCom_SetCOMErrorFromSimple(hr, IID_IEnumString);
}
