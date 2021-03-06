// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Copyright (c) 2017 Illumina, Inc.
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.

// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 *  \brief Test main file
 *
 * \file test_main.cpp
 * \author Peter Krusche
 * \email pkrusche@illumina.com
 *
 */

#include <cstdlib>
#include <libgen.h>
#include <limits.h>
#include <string>

#include "common.hh"
#include "common/Error.hh"
#include "gtest/gtest.h"

GTestEnvironment* g_testenv = nullptr;

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // initLogging("test_blackbox", "", false, "trace");

    // determine base path
    char* bn = dirname(argv[0]);
    char actualpath[PATH_MAX + 1];
    if (!realpath(bn, actualpath))
    {
        exit(1);
    }

    ::testing::AddGlobalTestEnvironment(g_testenv = new GTestEnvironment(actualpath));
    int res = RUN_ALL_TESTS();

    return res;
}
