/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DECODER_H__
#define __DECODER_H__

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "BenchmarkCommon.h"
#include "Extractor.h"
#include "Timer.h"

class Decoder : public CallBackHandle {
  public:
    Decoder()
        : mCodec(nullptr),
          mFormat(nullptr),
          mExtractor(nullptr),
          mTimer(nullptr),
          mNumInputFrame(0),
          mNumOutputFrame(0),
          mSawInputEOS(false),
          mSawOutputEOS(false),
          mSignalledError(false),
          mInputBuffer(nullptr) {
        mExtractor = new Extractor();
    }

    virtual ~Decoder() {
        if (mTimer) delete mTimer;
        if (mExtractor) delete mExtractor;
    }

    Timer *getTimer() override { return mTimer; }

    Extractor *getExtractor() { return mExtractor; }

    // Decoder related utilities
    void setupDecoder();

    void deInitCodec();

    void resetDecoder();

    // Async callback APIs
    void onInputAvailable(AMediaCodec *codec, int32_t index) override;

    void onFormatChanged(AMediaCodec *codec, AMediaFormat *format) override;

    void onOutputAvailable(AMediaCodec *codec, int32_t index,
                           AMediaCodecBufferInfo *bufferInfo) override;

    // Process the frames and give decoded output
    int32_t decode(uint8_t *inputBuffer, vector<AMediaCodecBufferInfo> &frameInfo,
                   string &codecName, bool asyncMode);

    void dumpStatistics(string inputReference);

  private:
    AMediaCodec *mCodec;
    AMediaFormat *mFormat;

    Extractor *mExtractor;

    Timer *mTimer;

    int32_t mNumInputFrame;
    int32_t mNumOutputFrame;

    bool mSawInputEOS;
    bool mSawOutputEOS;
    bool mSignalledError;

    int32_t mOffset;
    uint8_t *mInputBuffer;
    vector<AMediaCodecBufferInfo> mFrameMetaData;

    /* Asynchronous locks */
    mutex mMutex;
    condition_variable mDecoderDoneCondition;
};

// Read input samples
tuple<ssize_t, uint32_t, int64_t> readSampleData(uint8_t *inputBuffer, int32_t &offset,
                                                 vector<AMediaCodecBufferInfo> &frameSizes,
                                                 uint8_t *buf, int32_t frameID, size_t bufSize);

#endif  // __DECODER_H__
