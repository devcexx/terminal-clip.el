#include <cstddef>
#include <emacs-module.h>
#include <iterator>
#include <iostream>
#include <string>
#include "clip/clip.h"

int plugin_is_GPL_compatible;

static std::string copy_from_emacs_string(emacs_env* env, emacs_value str) {
  ptrdiff_t arglen;
  env->copy_string_contents(env, str, NULL, &arglen);

  char buf[arglen];
  env->copy_string_contents(env, str, buf, &arglen);
  return std::string(buf, arglen - 1);
}

static std::string symbol_value(emacs_env* env, emacs_value symbol) {
  return copy_from_emacs_string(env, env->funcall(env, env->intern(env, "symbol-name"), 1, &symbol));
}

static emacs_value intern_t(emacs_env* env) {
  return env->intern(env, "t");
}

static emacs_value intern_string(emacs_env* env) {
  return env->intern(env, "string");
}

static emacs_value intern_nil(emacs_env* env) {
  return env->intern(env, "nil");
}

static emacs_value copy_to_emacs_string(emacs_env* env, std::string& str) {
  return env->make_string (env, str.c_str(), str.length());
}

static void signal_user_error(emacs_env* env, std::string& message) {
  emacs_value emessage = copy_to_emacs_string(env, message);
  env->non_local_exit_signal(env, env->intern(env, "user-error"), env->funcall(env, env->intern (env, "list"), 1, &emessage));
}

static bool assert_argtype(emacs_env* env, emacs_value arg, emacs_value type) {
  auto type_got = env->type_of(env, arg);
  
  if (!env->eq(env, type, type_got)) {
    std::string message("Unexpected type of parameter. Expected "
			+ symbol_value(env, type) + " but "
			+ symbol_value(env, type_got) + " got.");
    signal_user_error(env, message);
    return false;
  }
  return true;
}

static emacs_value define_function(emacs_env* env,
			    ptrdiff_t min_args,
			    ptrdiff_t max_args,
			    emacs_value (*function) (emacs_env* env,
						     ptrdiff_t nargs,
						     emacs_value args[],
						     void *) EMACS_NOEXCEPT,
			    const char* lisp_alias) {

  auto func = env->make_function(env, min_args, max_args, function, NULL, NULL);
  auto symbol = env->intern (env, lisp_alias);
  
  emacs_value args[] = {symbol, func};
  env->funcall(env, env->intern(env, "defalias"), 2, args);
  return symbol;
}

static void provide(emacs_env* env, const char* feature_name) {
  auto feature_symbol = env->intern(env, feature_name);
  env->funcall(env, env->intern(env, "provide"), 1, &feature_symbol);
}

static emacs_value read_clipboard_text(emacs_env* env, ptrdiff_t nargs, emacs_value args[], void* data) noexcept {
  std::string contents;
  
  if (!clip::get_text(contents)) {
    return intern_nil(env);
  }

  return env->make_string(env, contents.c_str(), contents.length());
}

static emacs_value write_clipboard_text(emacs_env* env, ptrdiff_t nargs, emacs_value args[], void* data) noexcept {
  auto contents = args[0];

  if (!assert_argtype(env, contents, intern_string(env))) {
    return NULL;
  }

  auto str = copy_from_emacs_string(env, contents);
  if (clip::set_text(str)) {
    return intern_t(env);
  } else {
    return intern_nil(env);
  }
}

int emacs_module_init (struct emacs_runtime* ert) noexcept {
  auto *env = ert->get_environment(ert);
  define_function(env, 0, 0, read_clipboard_text, "terminal-clip-read-clipboard-text");
  define_function(env, 1, 1, write_clipboard_text, "terminal-clip-write-clipboard-text");

  provide(env, "terminal-clip-native");
  return 0;
}
