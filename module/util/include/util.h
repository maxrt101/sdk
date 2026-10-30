#pragma once

#include <util/str.h>
#include <util/base.h>
#include <util/vargs.h>
#include <util/cond.h>
#include <util/iter.h>
#include <util/compiler.h>
#include <util/assert.h>

#define has_module(__name) __has_module_##__name
#define is_platform(__name) __is_platform_##__name
#define is_target(__name) __is_target_##__name
