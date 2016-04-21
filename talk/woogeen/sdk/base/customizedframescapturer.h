/*
 * libjingle
 * Copyright 2010 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WOOGEEN_BASE_CUSTOMIZEDFRAMESCAPTURER_H_
#define WOOGEEN_BASE_CUSTOMIZEDFRAMESCAPTURER_H_

#include <string>
#include <vector>
#if defined(WEBRTC_WIN) || defined(WEBRTC_LINUX)
#include <memory>
#endif
#include "webrtc/media/base/videocapturer.h"
#include "webrtc/base/stream.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/base/bind.h"
#include "webrtc/base/asyncinvoker.h"
#include "webrtc/base/criticalsection.h"
#include "woogeen/base/framegeneratorinterface.h"

namespace woogeen {
namespace base {
using namespace cricket;
// Simulated video capturer that periodically reads frames from a file.
class CustomizedFramesCapturer : public VideoCapturer {
 public:
  CustomizedFramesCapturer(VideoFrameGeneratorInterface* rawFrameGenerator);
  virtual ~CustomizedFramesCapturer();
  static const char* kRawFrameDeviceName;

  void Init();
  void SendCapturedFrame();
  // Override virtual methods of parent class VideoCapturer.
  virtual CaptureState Start(
      const cricket::VideoFormat& capture_format) override;
  virtual void Stop() override;
  virtual bool IsRunning() override;
  virtual bool IsScreencast() const { return false; }

 protected:
  // Override virtual methods of parent class VideoCapturer.
  virtual bool GetPreferredFourccs(std::vector<uint32_t>* fourccs);

  // Read a frame and determine how long to wait for the next frame.
  virtual void ReadFrame();
  // Adjust |frame_buffer_|'s capacity to store frame data. |frame_buffer_|'s
  // capacity should be greater or equal to |size|.
  virtual void AdjustFrameBuffer(uint32_t size);

 private:
  class CustomizedFramesThread;  // Forward declaration, defined in .cc.

  VideoFrameGeneratorInterface* frame_generator_;
  CapturedFrame captured_frame_;
  CustomizedFramesThread* frames_generator_thread;
  int width_;
  int height_;
  int fps_;
  VideoFrameGeneratorInterface::VideoFrameCodec frame_type_;
  uint32_t frame_buffer_capacity_;
  std::unique_ptr<uint8_t[], webrtc::AlignedFreeDeleter>
      frame_buffer_;  // Pointer to a reuseable memory for video frames.
  rtc::Thread* worker_thread_;  // Set in Start(), unset in Stop();
  rtc::scoped_ptr<rtc::AsyncInvoker> async_invoker_;
  rtc::CriticalSection lock_;

  RTC_DISALLOW_COPY_AND_ASSIGN(CustomizedFramesCapturer);
};

}  // namespace base
}  // namespace woogeen

#endif  // WOOGEEN_BASE_CUSTOMIZEDFRAMESCAPTURER_H_