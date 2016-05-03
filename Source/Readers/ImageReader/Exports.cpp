//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// Exports.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define DATAREADER_EXPORTS
#include "DataReader.h"
#include "ReaderShim.h"
#include "ImageReader.h"
#include "HeapMemoryProvider.h"
#include "ImageDataDeserializer.h"
#include "ImageSlimTransformers.h"
#include "CorpusDescriptor.h"

namespace Microsoft { namespace MSR { namespace CNTK {

// TODO: Memory provider should be injected by SGD.

auto factory = [](const ConfigParameters& parameters) -> ReaderPtr
{
    return std::make_shared<ImageReader>(std::make_shared<HeapMemoryProvider>(), parameters);
};

extern "C" DATAREADER_API void GetReaderF(IDataReader** preader)
{
    *preader = new ReaderShim<float>(factory);
}

extern "C" DATAREADER_API void GetReaderD(IDataReader** preader)
{
    *preader = new ReaderShim<double>(factory);
}

// TODO: Not safe from the ABI perspective. Will be uglified to make the interface ABI.
extern "C" DATAREADER_API bool CreateDeserializer(IDataDeserializer** deserializer, const std::wstring& type, const ConfigParameters& deserializerConfig, CorpusDescriptorPtr corpus, bool)
{
    if (type == L"ImageDataDeserializer")
    {
        *deserializer = new ImageDataDeserializer(corpus, deserializerConfig);
    }
    else
    {
        // Unknown type.
        return false;
    }

    // Deserializer created.
    return true;
}

extern "C" DATAREADER_API bool CreateTransformer(SlimTransformer** transformer, const std::wstring& type, const ConfigParameters& config)
{
    if (type == L"Crop")
    {
        *transformer = new SlimCropTransformer(config);
    }
    else if (type == L"Scale")
    {
        *transformer = new SlimScaleTransformer(config);
    }
    else if (type == L"Color")
    {
        *transformer = new SlimColorTransformer(config);
    }
    else if (type == L"Intensity")
    {
        *transformer = new SlimIntensityTransformer(config);
    }
    else if (type == L"Mean")
    {
        *transformer = new SlimMeanTransformer(config);
    }
    else if (type == L"Transpose")
    {
        *transformer = new SlimTransposeTransformer(config);
    }
    else
    {
        // Unknown type.
        return false;
    }

    // Deserializer created.
    return true;
}

}}}
