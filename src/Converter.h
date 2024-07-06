// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Converter.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#ifndef CONVERTER_H
#define CONVERTER_H

void convertKoalaToPNG(const char *inputFile, const char *outputFile);
void convertPNGToKoala(const char *inputFile, const char *outputFile);

#endif // CONVERTER_H