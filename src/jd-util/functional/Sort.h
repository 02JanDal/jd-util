/* Copyright 2016 Jan Dalheimer <jan@dalheimer.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <algorithm>

#include "Base.h"
#include "ContainerTraits.h"
#include "FunctionTraits.h"

namespace JD {
namespace Util {
namespace Functional {

template <typename Container, typename Func>
auto sort(const Container &input, Func &&func, std::enable_if_t<FunctionTraits<Func>::arity == 2>* = nullptr)
{
	static_assert(std::is_same<typename FunctionTraits<Func>::ReturnType, bool>::value, "error: func needs to return a boolean value");
	Container output = input;
	std::sort(std::begin(output), std::end(output), std::forward<Func>(func));
	return output;
}

template <typename Container>
auto sort(const Container &input)
{
	Container output = input;
	std::sort(std::begin(output), std::end(output));
	return output;
}
}
}
}
