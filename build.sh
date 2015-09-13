#!/usr/bin/env bash

clang++ -shared -g -fno-exceptions -fno-rtti -fno-common src/plugin.cpp -o dumpNames.so -lclangFrontend -lclangDriver -lclangCodeGen -lclangSema -lclangAnalysis -lclangRewrite -lclangAST -lclangParse -lclangLex -lclangBasic -lLLVMSupport `llvm-config-3.5 --cxxflags --ldflags --libs all --system-libs`
