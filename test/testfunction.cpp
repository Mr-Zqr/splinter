/*
 * This file is part of the SPLINTER library.
 * Copyright (C) 2012 Bjarne Grimstad (bjarne.grimstad@gmail.com).
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <testfunction.h>
#include <test_functions.h>
#include <testingutilities.h>

using namespace SPLINTER;

TestFunction::TestFunction(Term *func)
        : f(func->simplify())
{
#ifndef NDEBUG
    std::cout << "f: ";
    f->pretty_text(std::cout); std::cout << std::endl;
#endif

    gatherVariables();
    calculateJacobian();
    calculateHessian();
}

TestFunction::TestFunction(Term &func)
        : TestFunction(func.clone())
{
}

void TestFunction::gatherVariables()
{
    auto vars = f->getVariables();
    for(auto &var : vars) {
        variables.push_back(var);
    }
    sort(variables.begin(), variables.end());
    numVariables = variables.size();

#ifndef NDEBUG
    std::cout << "NumVariables: " << numVariables << std::endl;
#endif
}

double TestFunction::eval(DenseVector x) const
{
    auto xvec = denseToVec(x);

    return f->eval(xvec);
}

DenseMatrix TestFunction::evalJacobian(DenseVector x) const
{
    DenseMatrix jacobian(1, numVariables);

    auto xvec = denseToVec(x);

    for(size_t i = 0; i < numVariables; i++) {
        jacobian(0, i) = jac.at(i)->eval(xvec);
    }

    return jacobian;
}

DenseMatrix TestFunction::evalHessian(DenseVector x) const
{
    DenseMatrix hessian(numVariables, numVariables);

    auto xvec = denseToVec(x);

    for(size_t i = 0; i < numVariables; i++) {
        for(size_t j = 0; j < numVariables; j++) {
            hessian(i, j) = hes.at(i).at(j)->eval(xvec);
        }
    }

    return hessian;
}

void TestFunction::calculateJacobian()
{
    jac.clear();
    for(size_t i = 0; i < numVariables; i++) {
        auto dx = variables.at(i);
        auto dfdx = f->derivative(dx);

#ifndef NDEBUG
        std::cout << "df/d";
        dx.pretty_text(std::cout);
        std::cout << ": ";
        dfdx->pretty_text(std::cout);
        std::cout << std::endl;
#endif // ifndef NDEBUG

        auto simplified_dfdx = dfdx->simplify();

#ifndef NDEBUG
        std::cout << "simplified df/d";
        dx.pretty_text(std::cout);
        std::cout << ": ";

        simplified_dfdx->pretty_text(std::cout);
        std::cout << std::endl;
        std::cout << std::endl;
#endif // ifndef NDEBUG

        jac.push_back(simplified_dfdx);
    }
}

void TestFunction::calculateHessian()
{
    hes.clear();

    for(size_t i = 0; i < numVariables; i++) {
        hes.push_back(std::vector<Term *>());

        for(size_t j = 0; j < numVariables; j++) {
            Var dx = variables.at(j);
            auto ddf = jac.at(i)->derivative(dx);

#ifndef NDEBUG
            std::cout << "d^2f/d";
            variables.at(i).pretty_text(std::cout);
            std::cout << "d";
            dx.pretty_text(std::cout);
            std::cout << ": ";
            ddf->pretty_text(std::cout);
            std::cout << std::endl;
#endif // ifndef NDEBUG

            auto simplified_ddf = ddf->simplify();

#ifndef NDEBUG
            std::cout << "simplified d^2f/d";
            variables.at(i).pretty_text(std::cout);
            if(i == j) {
                std::cout << "^2";
            } else {
                std::cout << "d";
                dx.pretty_text(std::cout);
            }
            std::cout << ": ";
            simplified_ddf->pretty_text(std::cout);
            std::cout << std::endl;
            std::cout << std::endl;
#endif // ifndef NDEBUG

            hes.at(i).push_back(simplified_ddf);
        }
    }
}
