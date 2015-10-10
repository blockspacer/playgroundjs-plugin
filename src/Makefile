# Copyright 2015 Las Venturas Playground. All rights reserved.
# Use of this source code is governed by the MIT license, a copy of which can
# be found in the LICENSE file.

$(shell [ -d "out" ] || mkdir -p out)
$(shell [ -d "out/obj/" ] || mkdir -p out/obj)

V8LIBS=v8/out/ia32.release/obj.target/tools/gyp

C=clang
CC=clang++
CFLAGS=-c -m32 -fPIC -O3 -std=gnu++0x -w -DLINUX -DNDEBUG -I. -Iplayground -Iv8
FLAGS=-c -m32 -fPIC -O3 -w -DLINUX -I. -Iplayground -Iv8

OUTFILE=playgroundjs-plugin.so

# Target: /
all: playground move link

# Target: /playground
playground: playground_base playground_bindings playground_plugin playground_third_party
	$(CC) $(CFLAGS) playground/playground_controller.cc -o out/obj/playground_playground_controller.o

# Target: /playground/base/
playground_base:
	$(CC) $(CFLAGS) playground/base/file_path.cc -o out/obj/playground_base_file_path.o
	$(CC) $(CFLAGS) playground/base/logging.cc -o out/obj/playground_base_logging.o
	$(CC) $(CFLAGS) playground/base/string_piece.cc -o out/obj/playground_base_string_piece.o
	$(CC) $(CFLAGS) playground/base/time.cc -o out/obj/playground_base_time.o

# Target: /playground/bindings/
playground_bindings:
	$(CC) $(CFLAGS) playground/bindings/console.cc -o out/obj/playground_bindings_console.o
	$(CC) $(CFLAGS) playground/bindings/event.cc -o out/obj/playground_bindings_event.o
	$(CC) $(CFLAGS) playground/bindings/exception_handler.cc -o out/obj/playground_bindings_exception_handler.o
	$(CC) $(CFLAGS) playground/bindings/global_callbacks.cc -o out/obj/playground_bindings_global_callbacks.o
	$(CC) $(CFLAGS) playground/bindings/global_scope.cc -o out/obj/playground_bindings_global_scope.o
	$(CC) $(CFLAGS) playground/bindings/pawn_invoke.cc -o out/obj/playground_bindings_pawn_invoke.o
	$(CC) $(CFLAGS) playground/bindings/runtime.cc -o out/obj/playground_bindings_runtime.o
	$(CC) $(CFLAGS) playground/bindings/runtime_operations.cc -o out/obj/playground_bindings_runtime_operations.o
	$(CC) $(CFLAGS) playground/bindings/script_prologue.cc -o out/obj/playground_bindings_script_prologue.o

# Target: /playground/plugin/sdk/
playground_plugin_sdk:
	$(CC) $(CFLAGS) playground/plugin/sdk/amxplugin.cpp -o out/obj/playground_plugin_sdk_amxplugin.o

# Target: /playground/plugin/
playground_plugin: playground_plugin_sdk
	$(CC) $(CFLAGS) playground/plugin/arguments.cc -o out/obj/playground_plugin_arguments.o
	$(CC) $(CFLAGS) playground/plugin/callback_hook.cc -o out/obj/playground_plugin_callback_hook.o
	$(CC) $(CFLAGS) playground/plugin/callback_parser.cc -o out/obj/playground_plugin_callback_parser.o
	$(CC) $(CFLAGS) playground/plugin/fake_amx.cc -o out/obj/playground_plugin_fake_amx.o
	$(CC) $(CFLAGS) playground/plugin/native_function_manager.cc -o out/obj/playground_plugin_native_function_manager.o
	$(CC) $(CFLAGS) playground/plugin/pawn_helpers.cc -o out/obj/playground_plugin_pawn_helpers.o
	$(CC) $(CFLAGS) playground/plugin/plugin.cc -o out/obj/playground_plugin_plugin.o
	$(CC) $(CFLAGS) playground/plugin/plugin_controller.cc -o out/obj/playground_plugin_plugin_controller.o

# Target: /playground/third_party/subhook/
playground_third_party_subhook:
	$(C) $(FLAGS) playground/third_party/subhook/subhook.c -o out/obj/playground_third_party_subhook_subhook.o
	$(C) $(FLAGS) playground/third_party/subhook/subhook_linux.c -o out/obj/playground_third_party_subhook_subhook_linux.o
	$(C) $(FLAGS) playground/third_party/subhook/subhook_x86.c -o out/obj/playground_third_party_subhook_x86.o

# Target: /playground/third_party/
playground_third_party: playground_third_party_subhook

# Link playground.so
link:
	$(CC) -O2 -m32 -fshort-wchar -shared -o out/$(OUTFILE) \
		out/obj/*.o \
		$(V8LIBS)/libv8_libplatform.a \
		$(V8LIBS)/libv8_libbase.a \
		$(V8LIBS)/libv8_nosnapshot.a \
		out/libv8.so

# Move libv8.so
move:
	cp v8/out/ia32.release/lib.target/libv8.so out/libv8.so
	strip out/libv8.so

# Clean
clean:
	rm -rf out