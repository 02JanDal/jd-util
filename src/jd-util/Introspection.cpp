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

#include "Introspection.h"

#include <QString>

QString JD::Util::Introspection::detail::extractClass(const char *func)
{
	const QString in = QString::fromLatin1(func);
	// given an input like "void Namespace::Class::method()" it first gets the part between the last : and first space
	// TODO: handle : in the parameter list
	return in.left(in.lastIndexOf(QLatin1Char(':'))-1).mid(in.lastIndexOf(QLatin1Char(' '))+1);
}
