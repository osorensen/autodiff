//                  _  _
//  _   _|_ _  _|o_|__|_
// (_||_||_(_)(_|| |  |
//
// automatic differentiation made easier in C++
// https://github.com/autodiff/autodiff
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
//
// Copyright (c) 2018-2020 Allan Leal
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Eigen includes
#include <Eigen/Core>

// pybind11 includes
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
namespace py = pybind11;

template<typename Vec, typename T, bool isarray, bool isconst, bool isview>
void exportVector(py::module& m, const char* typestr)
{
    auto cls = py::class_<Vec>(m, typestr);

    if constexpr (!isview) {
        using VecRef = Eigen::Ref<Vec>;
        using VecConstRef = Eigen::Ref<const Vec>;

        cls.def(py::init<>());
        cls.def(py::init<long>());
        cls.def(py::init<const VecRef&>());
        cls.def(py::init<const VecConstRef&>());
    }

    // Define constructors if not a view (e.g., wrapped into an Eigen::Ref)
    if constexpr (!isview) {
        cls.def(py::init<>());
        cls.def(py::init<long>());
    }

    cls.def("__len__", [](const Vec& s) { return s.size(); });

    cls.def("__getitem__", [](const Vec& s, size_t i) {
        const size_t size = s.size();
        if(i >= size) throw py::index_error();
        return s[i];
    });

    if constexpr (!isconst) {
        cls.def("__setitem__", [](Vec& s, size_t i, const T& val) {
            const size_t size = s.size();
            if(i >= size) throw py::index_error();
            s[i] = val;
        });
    }

#if EIGEN_VERSION_AT_LEAST(3, 3, 90)
    cls.def("__iter__", [](const Vec& s) {
        return py::make_iterator(s.begin(), s.end()); // begin/end iterators have not always being available!
    }, py::keep_alive<0, 1>()); // keep object alive while iterator exists;
#endif

    cls.def("__str__", [](const Vec& s) {
        std::stringstream stream;
        stream << s;
        return stream.str();
    });

    cls.def(-py::self);

    cls.def(py::self + py::self);
    cls.def(py::self - py::self);

    if constexpr(!isconst) {
        cls.def(py::self += py::self);
        cls.def(py::self -= py::self);
        cls.def(py::self *= T());
        cls.def(py::self /= T());
    }

    if constexpr (!isarray) {
        cls.def(py::self == py::self);
        cls.def(py::self != py::self);
    }

    if constexpr (isarray) {
        cls.def(py::self * py::self);
        cls.def(py::self / py::self);

        cls.def(py::self + T());
        cls.def(py::self - T());
        cls.def(py::self * T());
        cls.def(py::self / T());

        cls.def(T() + py::self);
        cls.def(T() - py::self);
        cls.def(T() * py::self);
        cls.def(T() / py::self);

        if constexpr (!isconst) {
            cls.def(py::self *= py::self);
            cls.def(py::self /= py::self);
            cls.def(py::self += T());
            cls.def(py::self -= T());
        }
    }
}

constexpr auto isview(bool val) { return val; }
constexpr auto isconst(bool val) { return val; }
constexpr auto isarray(bool val) { return val; }
