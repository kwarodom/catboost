#include <library/unittest/registar.h>
#include <iostream>
#include <catboost/cuda/cuda_util/cpu_random.h>
#include <library/threading/local_executor/local_executor.h>
#include <catboost/cuda/cuda_util/compression_helpers.h>

using namespace std;

SIMPLE_UNIT_TEST_SUITE(TCompressionTest) {
    SIMPLE_UNIT_TEST(TestCompressAndDecompress) {
        {
            NPar::LocalExecutor().RunAdditionalThreads(8);

            ui64 bits = 25;
            TRandom rand(0);

            for (ui32 bitsPerKey = 1; bitsPerKey < bits; ++bitsPerKey) {
                yvector<ui32> vec;
                ui32 uniqueValues = (1 << bitsPerKey);

                ui64 size = rand.NextUniformL() % 1000000;
                for (ui64 i = 0; i < size; ++i) {
                    vec.push_back(rand.NextUniformL() % uniqueValues);
                }

                auto compressed = CompressVector<ui64, ui32>(vec, bitsPerKey);
                auto decompressed = DecompressVector<ui64, ui32>(compressed, size, bitsPerKey);

                for (ui32 i = 0; i < size; ++i) {
                    UNIT_ASSERT_VALUES_EQUAL(decompressed[i], vec[i]);
                }
            }
        }
    }
}
