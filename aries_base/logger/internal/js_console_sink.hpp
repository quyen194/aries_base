/********************************************************************
  Copyright 2026, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/01/30 16:47
  filename:  aries_base/logger/internal/js_console_sink.hpp

  purpose:   Custom spdlog sink that forwards formatted log messages
             directly to the browser's JavaScript console (DevTools)
             when compiling with Emscripten (WebAssembly target).
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_LOGGER_INTERNAL_JS_CONSOLE_SINK_HPP
#define ARIES_BASE_LOGGER_INTERNAL_JS_CONSOLE_SINK_HPP
// -----------------------------------------------------------------------------
#ifdef __EMSCRIPTEN__
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <mutex>
#include <string>

#include <emscripten/emscripten.h>

#include <spdlog/details/log_msg.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/base_sink.h>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace common {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// spdlog sink writing formatted messages to the browser console.
// Use the _st variant for single-threaded builds (default Emscripten, no
// -s USE_PTHREADS=1) to avoid locking overhead.
template <typename Mutex>
class js_console_sink : public spdlog::sinks::base_sink<Mutex> {
 protected:
  void sink_it_(const spdlog::details::log_msg& msg) override {
    // Format with the sink's pattern
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

    // UTF8ToString() needs a null-terminated string
    std::string log_str = fmt::to_string(formatted);

    EM_ASM({ console.log(UTF8ToString($0)); }, log_str.c_str());
  }

  // Browser console flushes automatically
  void flush_() override {}
};

using js_console_sink_mt = js_console_sink<std::mutex>;
using js_console_sink_st = js_console_sink<spdlog::details::null_mutex>;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace common
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif // __EMSCRIPTEN__
// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_LOGGER_INTERNAL_JS_CONSOLE_SINK_HPP
// -----------------------------------------------------------------------------
