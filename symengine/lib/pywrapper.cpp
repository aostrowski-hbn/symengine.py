#include "pywrapper.h"
#include <symengine/serialize-cereal.h>

#if PY_MAJOR_VERSION >= 3
#define PyInt_FromLong PyLong_FromLong
#define PyNumber_Divide PyNumber_TrueDivide
#endif

namespace SymEngine {

// PyModule
PyModule::PyModule(PyObject* (*to_py)(const RCP<const Basic>), RCP<const Basic> (*from_py)(PyObject*),
                   RCP<const Number> (*eval)(PyObject*, long), RCP<const Basic> (*diff)(PyObject*, RCP<const Basic>)) :
        to_py_(to_py), from_py_(from_py), eval_(eval), diff_(diff) {
    zero = PyInt_FromLong(0);
    one = PyInt_FromLong(1);
    minus_one = PyInt_FromLong(-1);
}

PyModule::~PyModule(){
    Py_DECREF(zero);
    Py_DECREF(one);
    Py_DECREF(minus_one);
}

// PyNumber
PyNumber::PyNumber(PyObject* pyobject, const RCP<const PyModule> &pymodule) :
        pyobject_(pyobject), pymodule_(pymodule) {
}

hash_t PyNumber::__hash__() const {
    return PyObject_Hash(pyobject_);
}

bool PyNumber::__eq__(const Basic &o) const {
    return is_a<PyNumber>(o) and
        PyObject_RichCompareBool(pyobject_, static_cast<const PyNumber &>(o).get_py_object(), Py_EQ) == 1;
}

int PyNumber::compare(const Basic &o) const {
    SYMENGINE_ASSERT(is_a<PyNumber>(o))
    PyObject* o1 = static_cast<const PyNumber &>(o).get_py_object();
    if (PyObject_RichCompareBool(pyobject_, o1, Py_EQ) == 1)
        return 0;
    return PyObject_RichCompareBool(pyobject_, o1, Py_LT) == 1 ? -1 : 1;
}

bool PyNumber::is_zero() const {
    return PyObject_RichCompareBool(pyobject_, pymodule_->get_zero(), Py_EQ) == 1;
}
//! \return true if `1`
bool PyNumber::is_one() const {
    return PyObject_RichCompareBool(pyobject_, pymodule_->get_one(), Py_EQ) == 1;
}
//! \return true if `-1`
bool PyNumber::is_minus_one() const {
    return PyObject_RichCompareBool(pyobject_, pymodule_->get_minus_one(), Py_EQ) == 1;
}
//! \return true if negative
bool PyNumber::is_negative() const {
    return PyObject_RichCompareBool(pyobject_, pymodule_->get_zero(), Py_LT) == 1;
}
//! \return true if positive
bool PyNumber::is_positive() const {
    return PyObject_RichCompareBool(pyobject_, pymodule_->get_zero(), Py_GT) == 1;
}
//! \return true if complex
bool PyNumber::is_complex() const {
    return false;
}

//! Addition
RCP<const Number> PyNumber::add(const Number &other) const {
    PyObject *other_p, *result;
    if (is_a<PyNumber>(other)) {
        other_p = static_cast<const PyNumber &>(other).pyobject_;
        result = PyNumber_Add(pyobject_, other_p);
    } else {
        other_p = pymodule_->to_py_(other.rcp_from_this_cast<const Basic>());
        result = PyNumber_Add(pyobject_, other_p);
        Py_XDECREF(other_p);
    }
    return make_rcp<PyNumber>(result, pymodule_);
}
//! Subtraction
RCP<const Number> PyNumber::sub(const Number &other) const {
    PyObject *other_p, *result;
    if (is_a<PyNumber>(other)) {
        other_p = static_cast<const PyNumber &>(other).pyobject_;
        result = PyNumber_Subtract(pyobject_, other_p);
    } else {
        other_p = pymodule_->to_py_(other.rcp_from_this_cast<const Basic>());
        result = PyNumber_Subtract(pyobject_, other_p);
        Py_XDECREF(other_p);
    }
    return make_rcp<PyNumber>(result, pymodule_);
}
RCP<const Number> PyNumber::rsub(const Number &other) const {
    PyObject *other_p, *result;
    if (is_a<PyNumber>(other)) {
        other_p = static_cast<const PyNumber &>(other).pyobject_;
        result = PyNumber_Subtract(other_p, pyobject_);
    } else {
        other_p = pymodule_->to_py_(other.rcp_from_this_cast<const Basic>());
        result = PyNumber_Subtract(other_p, pyobject_);
        Py_XDECREF(other_p);
    }
    return make_rcp<PyNumber>(result, pymodule_);
}
//! Multiplication
RCP<const Number> PyNumber::mul(const Number &other) const {
    PyObject *other_p, *result;
    if (is_a<PyNumber>(other)) {
        other_p = static_cast<const PyNumber &>(other).pyobject_;
        result = PyNumber_Multiply(pyobject_, other_p);
    } else {
        other_p = pymodule_->to_py_(other.rcp_from_this_cast<const Basic>());
        result = PyNumber_Multiply(pyobject_, other_p);
        Py_XDECREF(other_p);
    }
    return make_rcp<PyNumber>(result, pymodule_);
}
//! Division
RCP<const Number> PyNumber::div(const Number &other) const {
    PyObject *other_p, *result;
    if (is_a<PyNumber>(other)) {
        other_p = static_cast<const PyNumber &>(other).pyobject_;
        result = PyNumber_Divide(pyobject_, other_p);
    } else {
        other_p = pymodule_->to_py_(other.rcp_from_this_cast<const Basic>());
        result = PyNumber_Divide(pyobject_, other_p);
        Py_XDECREF(other_p);
    }
    return make_rcp<PyNumber>(result, pymodule_);
}
RCP<const Number> PyNumber::rdiv(const Number &other) const {
    PyObject *other_p, *result;
    if (is_a<PyNumber>(other)) {
        other_p = static_cast<const PyNumber &>(other).pyobject_;
        result = PyNumber_Divide(pyobject_, other_p);
    } else {
        other_p = pymodule_->to_py_(other.rcp_from_this_cast<const Basic>());
        result = PyNumber_Divide(pyobject_, other_p);
        Py_XDECREF(other_p);
    }
    return make_rcp<PyNumber>(result, pymodule_);
}
//! Power
RCP<const Number> PyNumber::pow(const Number &other) const {
    PyObject *other_p, *result;
    if (is_a<PyNumber>(other)) {
        other_p = static_cast<const PyNumber &>(other).pyobject_;
        result = PyNumber_Power(pyobject_, other_p, Py_None);
    } else {
        other_p = pymodule_->to_py_(other.rcp_from_this_cast<const Basic>());
        result = PyNumber_Power(pyobject_, other_p, Py_None);
        Py_XDECREF(other_p);
    }
    return make_rcp<PyNumber>(result, pymodule_);
}
RCP<const Number> PyNumber::rpow(const Number &other) const {
    PyObject *other_p, *result;
    if (is_a<PyNumber>(other)) {
        other_p = static_cast<const PyNumber &>(other).pyobject_;
        result = PyNumber_Power(other_p, pyobject_, Py_None);
    } else {
        other_p = pymodule_->to_py_(other.rcp_from_this_cast<const Basic>());
        result = PyNumber_Power(other_p, pyobject_, Py_None);
        Py_XDECREF(other_p);
    }
    return make_rcp<PyNumber>(result, pymodule_);
}

RCP<const Number> PyNumber::eval(long bits) const {
    return pymodule_->eval_(pyobject_, bits);
}

std::string PyNumber::__str__() const {
    Py_ssize_t size;
    PyObject *pystr = PyObject_Str(pyobject_);
    const char* data = PyUnicode_AsUTF8AndSize(pystr, &size);
    std::string result = std::string(data, size);
    Py_XDECREF(pystr);
    return result;
}

// PyFunctionClass

PyFunctionClass::PyFunctionClass(PyObject *pyobject, std::string name, const RCP<const PyModule> &pymodule) :
        pyobject_{pyobject}, name_{name}, pymodule_{pymodule} {

}

PyObject* PyFunctionClass::call(const vec_basic &vec) const {
    PyObject *tuple = PyTuple_New(vec.size());
    for (unsigned i = 0; i < vec.size(); i++) {
        PyTuple_SetItem(tuple, i, pymodule_->to_py_(vec[i]));
    }
    PyObject* result = PyObject_CallObject(pyobject_, tuple);
    Py_DECREF(tuple);
    return result;
}

bool PyFunctionClass::__eq__(const PyFunctionClass &x) const {
    return PyObject_RichCompareBool(pyobject_, x.pyobject_, Py_EQ) == 1;
}

int PyFunctionClass::compare(const PyFunctionClass &x) const {
    if (__eq__(x)) return 0;
    return PyObject_RichCompareBool(pyobject_, x.pyobject_, Py_LT) == 1 ? 1 : -1;
}

hash_t PyFunctionClass::hash() const {
    if (hash_ == 0)
        hash_ = PyObject_Hash(pyobject_);
    return hash_;
}

// PyFunction
PyFunction::PyFunction(const vec_basic &vec, const RCP<const PyFunctionClass> &pyfunc_class,
           PyObject *pyobject) : FunctionWrapper(pyfunc_class->get_name(), std::move(vec)),
           pyfunction_class_{pyfunc_class}, pyobject_{pyobject} {

}

PyFunction::~PyFunction() {
    Py_DECREF(pyobject_);
}

PyObject* PyFunction::get_py_object() const {
    return pyobject_;
}

RCP<const PyFunctionClass> PyFunction::get_pyfunction_class() const {
    return pyfunction_class_;
}

RCP<const Basic> PyFunction::create(const vec_basic &x) const {
    PyObject* pyobj = pyfunction_class_->call(x);
    RCP<const Basic> result = pyfunction_class_->get_py_module()->from_py_(pyobj);
    Py_XDECREF(pyobj);
    return result;
}

RCP<const Number> PyFunction::eval(long bits) const {
    return pyfunction_class_->get_py_module()->eval_(pyobject_, bits);
}

RCP<const Basic> PyFunction::diff_impl(const RCP<const Symbol> &s) const {
    return pyfunction_class_->get_py_module()->diff_(pyobject_, s);
}

hash_t PyFunction::__hash__() const {
    return PyObject_Hash(pyobject_);
}

bool PyFunction::__eq__(const Basic &o) const {
    if (is_a<PyFunction>(o) and
        pyfunction_class_->__eq__(*static_cast<const PyFunction &>(o).get_pyfunction_class()) and
        unified_eq(get_vec(), static_cast<const PyFunction &>(o).get_vec()))
        return true;
    return false;
}

int PyFunction::compare(const Basic &o) const {
    SYMENGINE_ASSERT(is_a<PyFunction>(o))
    const PyFunction &s = static_cast<const PyFunction &>(o);
    int cmp = pyfunction_class_->compare(*s.get_pyfunction_class());
    if (cmp != 0) return cmp;
    return unified_compare(get_vec(), s.get_vec());
}

inline PyObject* get_pickle_module() {
    static PyObject *module = NULL;
    if (module == NULL) {
        module = PyImport_ImportModule("pickle");
    }
    if (module == NULL) {
        throw SymEngineException("error importing pickle module.");
    }
    return module;
}

PyObject* pickle_loads(const std::string &pickle_str) {
    PyObject *module = get_pickle_module();
    PyObject *pickle_bytes = PyBytes_FromStringAndSize(pickle_str.data(), pickle_str.size());
    PyObject *obj = PyObject_CallMethod(module, "loads", "O", pickle_bytes);
    Py_XDECREF(pickle_bytes);
    if (obj == NULL) {
        throw SerializationError("error when loading pickled symbol subclass object");
    }
    return obj;
}

RCP<const Basic> load_basic(RCPBasicAwareInputArchive<cereal::PortableBinaryInputArchive> &ar, RCP<const Symbol> &)
{
    bool is_pysymbol;
    bool store_pickle;
    std::string name;
    ar(is_pysymbol);
    ar(name);
    if (is_pysymbol) {
        std::string pickle_str;
        ar(pickle_str);
        ar(store_pickle);
        PyObject *obj = pickle_loads(pickle_str);
        RCP<const Basic> result = make_rcp<PySymbol>(name, obj, store_pickle);
        Py_XDECREF(obj);
        return result;
    } else {
        return symbol(name);
    }
}

std::string pickle_dumps(const PyObject * obj) {
    PyObject *module = get_pickle_module();
    PyObject *pickle_bytes = PyObject_CallMethod(module, "dumps", "O", obj);
    if (pickle_bytes == NULL) {
        throw SerializationError("error when pickling symbol subclass object");
    }
    Py_ssize_t size;
    char* buffer;
    PyBytes_AsStringAndSize(pickle_bytes, &buffer, &size);
    return std::string(buffer, size);
}

void save_basic(RCPBasicAwareOutputArchive<cereal::PortableBinaryOutputArchive> &ar, const Symbol &b)
{
    bool is_pysymbol = is_a_sub<PySymbol>(b);
    ar(is_pysymbol);
    ar(b.__str__());
    if (is_pysymbol) {
        RCP<const PySymbol> p = rcp_static_cast<const PySymbol>(b.rcp_from_this());
        PyObject *obj = p->get_py_object();
        std::string pickle_str = pickle_dumps(obj);
        ar(pickle_str);
        ar(p->store_pickle);
        Py_XDECREF(obj);
    }
}

std::string wrapper_dumps(const Basic &x)
{
    std::ostringstream oss;
    unsigned short major = SYMENGINE_MAJOR_VERSION;
    unsigned short minor = SYMENGINE_MINOR_VERSION;
    RCPBasicAwareOutputArchive<cereal::PortableBinaryOutputArchive>{oss}(major, minor,
                                             x.rcp_from_this());
    return oss.str();
}

RCP<const Basic> wrapper_loads(const std::string &serialized)
{
    unsigned short major, minor;
    RCP<const Basic> obj;
    std::istringstream iss(serialized);
    RCPBasicAwareInputArchive<cereal::PortableBinaryInputArchive> iarchive{iss};
    iarchive(major, minor);
    if (major != SYMENGINE_MAJOR_VERSION or minor != SYMENGINE_MINOR_VERSION) {
        throw SerializationError(StreamFmt()
                                 << "SymEngine-" << SYMENGINE_MAJOR_VERSION
                                 << "." << SYMENGINE_MINOR_VERSION
                                 << " was asked to deserialize an object "
                                 << "created using SymEngine-" << major << "."
                                 << minor << ".");
    }
    iarchive(obj);
    return obj;
}

} // SymEngine
