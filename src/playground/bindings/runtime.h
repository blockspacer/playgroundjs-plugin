// Copyright 2015 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the MIT license, a copy of which can
// be found in the LICENSE file.

#ifndef PLAYGROUND_BINDINGS_RUNTIME_H_
#define PLAYGROUND_BINDINGS_RUNTIME_H_

#include <memory>
#include <string>
#include <unordered_set>

#include <include/v8.h>

#include "base/file_path.h"

namespace boost {
namespace asio {
class io_context;
}
}

namespace plugin {
class PluginController;
}

namespace bindings {

class ExceptionHandler;
class FrameObserver;
class GlobalScope;
class Profiler;
class RuntimeModulator;
class TimerQueue;

// The runtime class represents a v8 virtual machine. It must be externally owned, but additional
// references may be retrieved by the v8 Isolate it's keyed on.
class Runtime {
 public:
  // The delegate enables the runtime to communicate with its embedder for the purposes of passing
  // forward output that has been generated by the JavaScript engine.
  class Delegate {
   public:
    virtual void OnScriptOutput(const std::string& message) = 0;
    virtual void OnScriptError(const std::string& filename, size_t line_number, const std::string& message) = 0;
    virtual void OnScriptTestsDone(unsigned int total_tests, unsigned int failed_tests) = 0;

   protected:
    virtual ~Delegate() {}
  };

  // Returns the Runtime instance associated with |isolate|. May be a nullptr.
  static std::shared_ptr<Runtime> FromIsolate(v8::Isolate* isolate);

  // Creates a new instance of the Runtime based on |options|, optionally with |runtime_delegate|.
  static std::shared_ptr<Runtime> Create(Delegate* runtime_delegate,
                                         plugin::PluginController* plugin_controller);

  ~Runtime();

  // Initializes the runtime by installing all prototypes and objects. The global scope should have
  // been fully initialized prior to this call.
  void Initialize();

  // Spin the JavaScript engine until the ready flag has been set. This may never return in case a
  // problem with the JavaScript code has been found.
  void SpinUntilReady();
  bool IsReady() const { return is_ready_; }
  void SetReady();

  // Returns the average number of frames per second since the last call to this method. The
  // information will be written to |*duration| and |*average_fps|, which must be given.
  void GetAndResetFrameCounter(double* duration, double* average_fps);

  // To be called once per server frame. Will call listening frame delegates of features that need
  // to be informed every frame in order to work correctly (e.g. for asynchronous work).
  void OnFrame();

  // Adds or removes a frame observer from the runtime. Users of this functionality should use a
  // ScopedFrameObserver rather than trying to do this manually.
  void AddFrameObserver(FrameObserver* observer);
  void RemoveFrameObserver(FrameObserver* observer);

  // Returns the root file path in which the JavaScript code lives.
  const base::FilePath& source_directory() const { return source_directory_; }

  // Returns the modulator that should be used for loading modules.
  RuntimeModulator* GetModulator() { return modulator_.get(); }

  // Returns the global scope associated with this runtime. May be used to get access to the event
  // target and instances of the common JavaScript objects.
  GlobalScope* GetGlobalScope() { return global_scope_.get(); }

  // Returns the Profiler that can instrument the runtime.
  Profiler* GetProfiler() { return profiler_.get(); }

  // Returns the timer queue associated with this runtime.
  TimerQueue* GetTimerQueue() { return timer_queue_.get(); }

  // Returns the exception handler. It will generate extensive and readable error messages that will
  // tremendously help developers towards solving the problems.
  ExceptionHandler* GetExceptionHandler() { return exception_handler_.get(); }

  // Returns the Boost IO Context. Ownership belongs to this object, but the context itself can be
  // modified by any user, as this is a requirement for asynchronous posting tasks to it.
  boost::asio::io_context& io_context() { return *io_context_; }

  // Encapsulates both the source-code of a script and the origin filename.
  struct ScriptSource {
    ScriptSource() = default;
    ScriptSource(const std::string& source)
        : source(source) {}

    std::string source;
    std::string filename;
  };

  // Returns the isolate associated with this runtime.
  v8::Isolate* isolate() const { return isolate_; }

  // Returns the context associated with this runtime.
  v8::Local<v8::Context> context() const { return context_.Get(isolate_); }

  // Returns the delegate for this runtime. May be a nullptr.
  Delegate* delegate() { return runtime_delegate_; }

 private:
  Runtime(Delegate* runtime_delegate, plugin::PluginController* plugin_controller);

  base::FilePath source_directory_;
  Delegate* runtime_delegate_;

  // Set of attached frame observers.
  std::unordered_set<FrameObserver*> frame_observers_;

  std::unique_ptr<RuntimeModulator> modulator_;

  std::unique_ptr<v8::Platform> platform_;
  std::unique_ptr<v8::ArrayBuffer::Allocator> allocator_;

  // Memory for the isolate is owned by V8.
  v8::Isolate* isolate_;

  // This plugin uses a single isolate, so maintain a global isolate scope.
  std::unique_ptr<v8::Isolate::Scope> isolate_scope_;

  // The context used by the plugin. There will only be one.
  v8::Persistent<v8::Context> context_;

  // The global scope that will service the runtime.
  std::unique_ptr<GlobalScope> global_scope_;

  // The profiler that's able to profile Las Venturas Playground.
  std::unique_ptr<Profiler> profiler_;

  // The timer queue is a prioritized queue of time-dependent promises.
  std::unique_ptr<TimerQueue> timer_queue_;

  // The exception handler for handling unhandled exceptions.
  std::unique_ptr<ExceptionHandler> exception_handler_;

  // The server's IO Context, allowing asynchronous Boost functionality to work. A single tick
  // of work is allowed to happen during each OnFrame() invocation.
  std::unique_ptr<boost::asio::io_context> io_context_;

  // Flag indicating whether the JavaScript code has properly loaded.
  bool is_ready_;

  // Very simple frame counter for the runtime, allowing further inspection of performance.
  double frame_counter_start_;
  int64_t frame_counter_;
};

}  // namespace

#endif  // PLAYGROUND_BINDINGS_RUNTIME_H_
