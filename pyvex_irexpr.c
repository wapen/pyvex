#include <Python.h>
#include <structmember.h>
#include <libvex.h>

#include "pyvex_enums.h"
#include "pyvex_types.h"
#include "pyvex_macros.h"

///////////////////////
// IRExpr base class //
///////////////////////

PYVEX_NEW(IRExpr)
PYVEX_DEALLOC(IRExpr)
PYVEX_METH_STANDARD(IRExpr)

static int
pyIRExpr_init(pyIRExpr *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRExpr);
	PyErr_SetString(VexException, "Base IRExpr creation not supported..");
	return -1;
}

PYVEX_SETTER(IRExpr, wrapped)
PYVEX_GETTER(IRExpr, wrapped)
PYVEX_ACCESSOR_ENUM(IRExpr, IRExpr, wrapped->tag, tag, IRExprTag)

static PyGetSetDef pyIRExpr_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRExpr, wrapped),
	PYVEX_ACCESSOR_DEF(IRExpr, tag),
	{NULL}
};

static PyObject *pyIRExpr_atomic(pyIRExpr* self)
{
	if (isIRAtom(self->wrapped)) { Py_RETURN_TRUE; }
	Py_RETURN_FALSE;
}

static PyMethodDef pyIRExpr_methods[] =
{
	PYVEX_METHDEF_STANDARD(IRExpr),
	{"atomic", (PyCFunction)pyIRExpr_atomic, METH_NOARGS, "Returns true if IRExpr is atomic (RdTmp or Const), false otherwise."},
	{NULL}
};

static PyMemberDef pyIRExpr_members[] = { {NULL} };
PYVEX_TYPEOBJECT(IRExpr);

// wrap functionality
PyObject *wrap_IRExpr(IRExpr *i)
{
	PyTypeObject *t = NULL;

	switch (i->tag)
	{
		//PYVEX_WRAPCASE(IRExpr, Iex_, Binder)
		PYVEX_WRAPCASE(IRExpr, Iex_, Get)
		//PYVEX_WRAPCASE(IRExpr, Iex_, GetI)
		PYVEX_WRAPCASE(IRExpr, Iex_, RdTmp)
		PYVEX_WRAPCASE(IRExpr, Iex_, Qop)
		PYVEX_WRAPCASE(IRExpr, Iex_, Triop)
		PYVEX_WRAPCASE(IRExpr, Iex_, Binop)
		PYVEX_WRAPCASE(IRExpr, Iex_, Unop)
		PYVEX_WRAPCASE(IRExpr, Iex_, Load)
		//PYVEX_WRAPCASE(IRExpr, Iex_, Const)
		//PYVEX_WRAPCASE(IRExpr, Iex_, Mux0X)
		//PYVEX_WRAPCASE(IRExpr, Iex_, CCall)
		default:
			fprintf(stderr, "PyVEX: Unknown/unsupported IRExprTag %s\n", IRExprTag_to_str(i->tag));
			t = &pyIRExprType;
	}

	PyObject *args = Py_BuildValue("");
	PyObject *kwargs = Py_BuildValue("{s:O}", "wrap", PyCapsule_New(i, "IRExpr", NULL));
	PyObject *o = PyObject_Call((PyObject *)t, args, kwargs);
	Py_DECREF(args); Py_DECREF(kwargs);
	return (PyObject *)o;
}

////////////////
// Get IRExpr //
////////////////

static int
pyIRExprGet_init(pyIRExpr *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRExpr);

	Int offset;
	IRType type;
	char *type_str;
	
	static char *kwlist[] = {"offset", "type", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "is|O", kwlist, &offset, &type_str, &wrap_object)) return -1;
	PYVEX_ENUM_FROMSTR(IRType, type, type_str, return -1);

	self->wrapped = IRExpr_Get(offset, type);
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRExprGet, IRExpr, wrapped->Iex.Get.offset, offset, "i")
PYVEX_ACCESSOR_ENUM(IRExprGet, IRExpr, wrapped->Iex.Get.ty, type, IRType)

static PyGetSetDef pyIRExprGet_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRExprGet, offset),
	PYVEX_ACCESSOR_DEF(IRExprGet, type),
	{NULL}
};

static PyMethodDef pyIRExprGet_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IRExprGet, IRExpr);

//////////////////
// RdTmp IRExpr //
//////////////////

static int
pyIRExprRdTmp_init(pyIRExpr *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRExpr);

	UInt tmp;
	static char *kwlist[] = {"tmp", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "I|O", kwlist, &tmp, &wrap_object)) return -1;

	self->wrapped = IRExpr_RdTmp(tmp);
	return 0;
}

PYVEX_ACCESSOR_BUILDVAL(IRExprRdTmp, IRExpr, wrapped->Iex.RdTmp.tmp, tmp, "I")

static PyGetSetDef pyIRExprRdTmp_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRExprRdTmp, tmp),
	{NULL}
};

static PyMethodDef pyIRExprRdTmp_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IRExprRdTmp, IRExpr);

//////////////////
// Qop IRExpr //
//////////////////

static int
pyIRExprQop_init(pyIRExpr *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRExpr);

	IROp op;
	const char *op_str;
	pyIRExpr *arg1;
	pyIRExpr *arg2;
	pyIRExpr *arg3;
	pyIRExpr *arg4;

	static char *kwlist[] = {"op", "arg1", "arg2", "arg3", "arg4", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sOOOO|O", kwlist, &op_str, &arg1, &arg2, &arg3, &arg4, &wrap_object)) return -1;
	PYVEX_ENUM_FROMSTR(IROp, op, op_str, return -1);
	PYVEX_CHECKTYPE(arg1, pyIRExprType, return -1);
	PYVEX_CHECKTYPE(arg2, pyIRExprType, return -1);
	PYVEX_CHECKTYPE(arg3, pyIRExprType, return -1);
	PYVEX_CHECKTYPE(arg4, pyIRExprType, return -1);

	self->wrapped = IRExpr_Qop(op, arg1->wrapped, arg2->wrapped, arg3->wrapped, arg4->wrapped);
	return 0;
}

PYVEX_ACCESSOR_ENUM(IRExprQop, IRExpr, wrapped->Iex.Qop.details->op, op, IROp)
PYVEX_ACCESSOR_WRAPPED(IRExprQop, IRExpr, wrapped->Iex.Qop.details->arg1, arg1, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRExprQop, IRExpr, wrapped->Iex.Qop.details->arg2, arg2, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRExprQop, IRExpr, wrapped->Iex.Qop.details->arg3, arg3, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRExprQop, IRExpr, wrapped->Iex.Qop.details->arg4, arg4, IRExpr)

PyObject *
pyIRExprQop_args(pyIRExpr* self)
{
	return Py_BuildValue("(OOOO)", wrap_IRExpr(self->wrapped->Iex.Qop.details->arg1),
				       wrap_IRExpr(self->wrapped->Iex.Qop.details->arg2),
				       wrap_IRExpr(self->wrapped->Iex.Qop.details->arg3),
				       wrap_IRExpr(self->wrapped->Iex.Qop.details->arg4));
}

static PyGetSetDef pyIRExprQop_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRExprQop, op),
	PYVEX_ACCESSOR_DEF(IRExprQop, arg1),
	PYVEX_ACCESSOR_DEF(IRExprQop, arg2),
	PYVEX_ACCESSOR_DEF(IRExprQop, arg3),
	PYVEX_ACCESSOR_DEF(IRExprQop, arg4),
	{NULL}
};

static PyMethodDef pyIRExprQop_methods[] = { {"args", (PyCFunction)pyIRExprQop_args, METH_NOARGS, "Returns the arguments of the Qop"}, {NULL} };
PYVEX_SUBTYPEOBJECT(IRExprQop, IRExpr);

//////////////////
// Triop IRExpr //
//////////////////

static int
pyIRExprTriop_init(pyIRExpr *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRExpr);

	IROp op;
	const char *op_str;
	pyIRExpr *arg1;
	pyIRExpr *arg2;
	pyIRExpr *arg3;

	static char *kwlist[] = {"op", "arg1", "arg2", "arg3", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sOOO|O", kwlist, &op_str, &arg1, &arg2, &arg3, &wrap_object)) return -1;
	PYVEX_ENUM_FROMSTR(IROp, op, op_str, return -1);
	PYVEX_CHECKTYPE(arg1, pyIRExprType, return -1);
	PYVEX_CHECKTYPE(arg2, pyIRExprType, return -1);
	PYVEX_CHECKTYPE(arg3, pyIRExprType, return -1);

	self->wrapped = IRExpr_Triop(op, arg1->wrapped, arg2->wrapped, arg3->wrapped);
	return 0;
}

PYVEX_ACCESSOR_ENUM(IRExprTriop, IRExpr, wrapped->Iex.Triop.details->op, op, IROp)
PYVEX_ACCESSOR_WRAPPED(IRExprTriop, IRExpr, wrapped->Iex.Triop.details->arg1, arg1, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRExprTriop, IRExpr, wrapped->Iex.Triop.details->arg2, arg2, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRExprTriop, IRExpr, wrapped->Iex.Triop.details->arg3, arg3, IRExpr)

PyObject *
pyIRExprTriop_args(pyIRExpr* self)
{
	return Py_BuildValue("(OOO)", wrap_IRExpr(self->wrapped->Iex.Triop.details->arg1),
				       wrap_IRExpr(self->wrapped->Iex.Triop.details->arg2),
				       wrap_IRExpr(self->wrapped->Iex.Triop.details->arg3));
}

static PyGetSetDef pyIRExprTriop_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRExprTriop, op),
	PYVEX_ACCESSOR_DEF(IRExprTriop, arg1),
	PYVEX_ACCESSOR_DEF(IRExprTriop, arg2),
	PYVEX_ACCESSOR_DEF(IRExprTriop, arg3),
	{NULL}
};

static PyMethodDef pyIRExprTriop_methods[] = { {"args", (PyCFunction)pyIRExprTriop_args, METH_NOARGS, "Returns the arguments of the Triop"}, {NULL} };
PYVEX_SUBTYPEOBJECT(IRExprTriop, IRExpr);

//////////////////
// Binop IRExpr //
//////////////////

static int
pyIRExprBinop_init(pyIRExpr *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRExpr);

	IROp op;
	const char *op_str;
	pyIRExpr *arg1;
	pyIRExpr *arg2;

	static char *kwlist[] = {"op", "arg1", "arg2", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sOO|O", kwlist, &op_str, &arg1, &arg2, &wrap_object)) return -1;
	PYVEX_ENUM_FROMSTR(IROp, op, op_str, return -1);
	PYVEX_CHECKTYPE(arg1, pyIRExprType, return -1);
	PYVEX_CHECKTYPE(arg2, pyIRExprType, return -1);

	self->wrapped = IRExpr_Binop(op, arg1->wrapped, arg2->wrapped);
	return 0;
}

PYVEX_ACCESSOR_ENUM(IRExprBinop, IRExpr, wrapped->Iex.Binop.op, op, IROp)
PYVEX_ACCESSOR_WRAPPED(IRExprBinop, IRExpr, wrapped->Iex.Binop.arg1, arg1, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRExprBinop, IRExpr, wrapped->Iex.Binop.arg2, arg2, IRExpr)

PyObject *
pyIRExprBinop_args(pyIRExpr* self)
{
	return Py_BuildValue("(OO)", wrap_IRExpr(self->wrapped->Iex.Binop.arg1),
				       wrap_IRExpr(self->wrapped->Iex.Binop.arg2));
}

static PyGetSetDef pyIRExprBinop_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRExprBinop, op),
	PYVEX_ACCESSOR_DEF(IRExprBinop, arg1),
	PYVEX_ACCESSOR_DEF(IRExprBinop, arg2),
	{NULL}
};

static PyMethodDef pyIRExprBinop_methods[] = { {"args", (PyCFunction)pyIRExprBinop_args, METH_NOARGS, "Returns the arguments of the Binop"}, {NULL} };
PYVEX_SUBTYPEOBJECT(IRExprBinop, IRExpr);

//////////////////
// Unop IRExpr //
//////////////////

static int
pyIRExprUnop_init(pyIRExpr *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRExpr);

	IROp op;
	const char *op_str;
	pyIRExpr *arg1;

	static char *kwlist[] = {"op", "arg1", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sO|O", kwlist, &op_str, &arg1, &wrap_object)) return -1;
	PYVEX_ENUM_FROMSTR(IROp, op, op_str, return -1);
	PYVEX_CHECKTYPE(arg1, pyIRExprType, return -1);

	self->wrapped = IRExpr_Unop(op, arg1->wrapped);
	return 0;
}

PYVEX_ACCESSOR_ENUM(IRExprUnop, IRExpr, wrapped->Iex.Unop.op, op, IROp)
PYVEX_ACCESSOR_WRAPPED(IRExprUnop, IRExpr, wrapped->Iex.Unop.arg, arg, IRExpr)
PYVEX_ACCESSOR_WRAPPED(IRExprUnop, IRExpr, wrapped->Iex.Unop.arg, arg1, IRExpr)

PyObject *
pyIRExprUnop_args(pyIRExpr* self)
{
	return Py_BuildValue("(O)", wrap_IRExpr(self->wrapped->Iex.Unop.arg));
}

static PyGetSetDef pyIRExprUnop_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRExprUnop, op),
	PYVEX_ACCESSOR_DEF(IRExprUnop, arg),
	PYVEX_ACCESSOR_DEF(IRExprUnop, arg1),
	{NULL}
};

static PyMethodDef pyIRExprUnop_methods[] = { {"args", (PyCFunction)pyIRExprUnop_args, METH_NOARGS, "Returns the arguments of the Unop"}, {NULL} };
PYVEX_SUBTYPEOBJECT(IRExprUnop, IRExpr);

//////////////////
// Load IRExpr //
//////////////////

static int
pyIRExprLoad_init(pyIRExpr *self, PyObject *args, PyObject *kwargs)
{
	PYVEX_WRAP_CONSTRUCTOR(IRExpr);

	IREndness endness; const char *endness_str;
	IRType type; const char *type_str;
	pyIRExpr *addr;

	static char *kwlist[] = {"endness", "type", "addr", "wrap", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ssO|O", kwlist, &endness_str, &type_str, &addr, &wrap_object)) return -1;
	PYVEX_ENUM_FROMSTR(IREndness, endness, endness_str, return -1);
	PYVEX_ENUM_FROMSTR(IRType, type, type_str, return -1);
	PYVEX_CHECKTYPE(addr, pyIRExprType, return -1);

	self->wrapped = IRExpr_Load(endness, type, addr->wrapped);
	return 0;
}

PYVEX_ACCESSOR_ENUM(IRExprLoad, IRExpr, wrapped->Iex.Load.end, endness, IREndness)
PYVEX_ACCESSOR_ENUM(IRExprLoad, IRExpr, wrapped->Iex.Load.ty, type, IRType)
PYVEX_ACCESSOR_WRAPPED(IRExprLoad, IRExpr, wrapped->Iex.Load.addr, addr, IRExpr)

static PyGetSetDef pyIRExprLoad_getseters[] =
{
	PYVEX_ACCESSOR_DEF(IRExprLoad, endness),
	PYVEX_ACCESSOR_DEF(IRExprLoad, type),
	PYVEX_ACCESSOR_DEF(IRExprLoad, addr),
	{NULL}
};

static PyMethodDef pyIRExprLoad_methods[] = { {NULL} };
PYVEX_SUBTYPEOBJECT(IRExprLoad, IRExpr);
