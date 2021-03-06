/* Copyright (c) 2018 Anakin Authors, Inc. All Rights Reserved.
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef ANAKIN_SABER_FUNCS_SATURATE_H
#define ANAKIN_SABER_FUNCS_SATURATE_H

#include "saber/core/common.h"
#include <cmath>
#include <limits.h>
namespace anakin {
namespace saber{

template<typename _Tp> static inline _Tp saturate_cast(uint8_t v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(int8_t v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(uint16_t v)   { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(int16_t v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(uint32_t v) { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(int32_t v)      { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(float v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(double v)   { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(int64_t v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(uint64_t v)   { return _Tp(v); }

template<> inline uint8_t saturate_cast<uint8_t >(int8_t v)        { return (uint8_t)std::max((int)v, 0); }
template<> inline uint8_t saturate_cast<uint8_t >(uint16_t v)       { return (uint8_t)std::min((unsigned)v, (unsigned)UCHAR_MAX); }
template<> inline uint8_t saturate_cast<uint8_t >(int v)          { return (uint8_t)((unsigned)v <= UCHAR_MAX ? v : v > 0 ? UCHAR_MAX : 0); }
template<> inline uint8_t saturate_cast<uint8_t >(short v)        { return saturate_cast<uint8_t >((int)v); }
template<> inline uint8_t saturate_cast<uint8_t >(unsigned v)     { return (uint8_t)std::min(v, (unsigned)UCHAR_MAX); }
template<> inline uint8_t saturate_cast<uint8_t >(float v)        { int iv = (int)roundf(v); return saturate_cast<uint8_t >(iv); }
template<> inline uint8_t saturate_cast<uint8_t >(double v)       { int iv = (int)round(v); return saturate_cast<uint8_t >(iv); }
template<> inline uint8_t saturate_cast<uint8_t >(int64_t v)        { return (uint8_t)((uint64_t)v <= (uint64_t)UCHAR_MAX ? v : v > 0 ? UCHAR_MAX : 0); }
template<> inline uint8_t saturate_cast<uint8_t >(uint64_t v)       { return (uint8_t)std::min(v, (uint64_t)UCHAR_MAX); }

template<> inline int8_t saturate_cast<int8_t >(uint8_t v)        { return (int8_t)std::min((int)v, SCHAR_MAX); }
template<> inline int8_t saturate_cast<int8_t >(uint16_t v)       { return (int8_t)std::min((unsigned)v, (unsigned)SCHAR_MAX); }
template<> inline int8_t saturate_cast<int8_t >(int v)          { return (int8_t)((unsigned)(v-SCHAR_MIN) <= (unsigned)UCHAR_MAX ? v : v > 0 ? SCHAR_MAX : SCHAR_MIN); }
template<> inline int8_t saturate_cast<int8_t >(short v)        { return saturate_cast<int8_t >((int)v); }
template<> inline int8_t saturate_cast<int8_t >(unsigned v)     { return (int8_t)std::min(v, (unsigned)SCHAR_MAX); }
template<> inline int8_t saturate_cast<int8_t >(float v)        { int iv = (int)roundf(v); return saturate_cast<int8_t >(iv); }
template<> inline int8_t saturate_cast<int8_t >(double v)       { int iv = (int)round(v); return saturate_cast<int8_t >(iv); }
template<> inline int8_t saturate_cast<int8_t >(int64_t v)        { return (int8_t)((uint64_t)((int64_t)v-SCHAR_MIN) <= (uint64_t)UCHAR_MAX ? v : v > 0 ? SCHAR_MAX : SCHAR_MIN); }
template<> inline int8_t saturate_cast<int8_t >(uint64_t v)       { return (int8_t)std::min(v, (uint64_t)SCHAR_MAX); }

template<> inline uint16_t saturate_cast<uint16_t >(int8_t v)      { return (uint16_t)std::max((int)v, 0); }
template<> inline uint16_t saturate_cast<uint16_t >(short v)      { return (uint16_t)std::max((int)v, 0); }
template<> inline uint16_t saturate_cast<uint16_t >(int v)        { return (uint16_t)((unsigned)v <= (unsigned)USHRT_MAX ? v : v > 0 ? USHRT_MAX : 0); }
template<> inline uint16_t saturate_cast<uint16_t >(unsigned v)   { return (uint16_t)std::min(v, (unsigned)USHRT_MAX); }
template<> inline uint16_t saturate_cast<uint16_t >(float v)      { int iv = (int)roundf(v); return saturate_cast<uint16_t >(iv); }
template<> inline uint16_t saturate_cast<uint16_t >(double v)     { int iv = (int)round(v); return saturate_cast<uint16_t >(iv); }
template<> inline uint16_t saturate_cast<uint16_t >(int64_t v)      { return (uint16_t)((uint64_t)v <= (uint64_t)USHRT_MAX ? v : v > 0 ? USHRT_MAX : 0); }
template<> inline uint16_t saturate_cast<uint16_t >(uint64_t v)     { return (uint16_t)std::min(v, (uint64_t)USHRT_MAX); }

template<> inline short saturate_cast<short>(uint16_t v)       { return (short)std::min((int)v, SHRT_MAX); }
template<> inline short saturate_cast<short>(int v)          { return (short)((unsigned)(v - SHRT_MIN) <= (unsigned)USHRT_MAX ? v : v > 0 ? SHRT_MAX : SHRT_MIN); }
template<> inline short saturate_cast<short>(unsigned v)     { return (short)std::min(v, (unsigned)SHRT_MAX); }
template<> inline short saturate_cast<short>(float v)        { int iv = (int)roundf(v); return saturate_cast<short>(iv); }
template<> inline short saturate_cast<short>(double v)       { int iv = (int)round(v); return saturate_cast<short>(iv); }
template<> inline short saturate_cast<short>(int64_t v)        { return (short)((uint64_t)((int64_t)v - SHRT_MIN) <= (uint64_t)USHRT_MAX ? v : v > 0 ? SHRT_MAX : SHRT_MIN); }
template<> inline short saturate_cast<short>(uint64_t v)       { return (short)std::min(v, (uint64_t)SHRT_MAX); }

template<> inline int saturate_cast<int>(unsigned v)         { return (int)std::min(v, (unsigned)INT_MAX); }
template<> inline int saturate_cast<int>(int64_t v)            { return (int)((uint64_t)(v - INT_MIN) <= (uint64_t)UINT_MAX ? v : v > 0 ? INT_MAX : INT_MIN); }
template<> inline int saturate_cast<int>(uint64_t v)           { return (int)std::min(v, (uint64_t)INT_MAX); }
template<> inline int saturate_cast<int>(float v)            { return (int)roundf(v); }
template<> inline int saturate_cast<int>(double v)           { return (int)round(v); }

template<> inline unsigned saturate_cast<unsigned>(int8_t v)  { return (unsigned)std::max(v, (int8_t)0); }
template<> inline unsigned saturate_cast<unsigned>(short v)  { return (unsigned)std::max(v, (short)0); }
template<> inline unsigned saturate_cast<unsigned>(int v)    { return (unsigned)std::max(v, (int)0); }
template<> inline unsigned saturate_cast<unsigned>(int64_t v)  { return (unsigned)((uint64_t)v <= (uint64_t)UINT_MAX ? v : v > 0 ? UINT_MAX : 0); }
template<> inline unsigned saturate_cast<unsigned>(uint64_t v) { return (unsigned)std::min(v, (uint64_t)UINT_MAX); }
// we intentionally do not clip negative numbers, to make -1 become 0xffffffff etc.
template<> inline unsigned saturate_cast<unsigned>(float v)  { return static_cast<unsigned>(roundf(v)); }
template<> inline unsigned saturate_cast<unsigned>(double v) { return static_cast<unsigned>(round(v)); }

template<> inline uint64_t saturate_cast<uint64_t>(int8_t v)      { return (uint64_t)std::max(v, (int8_t)0); }
template<> inline uint64_t saturate_cast<uint64_t>(short v)      { return (uint64_t)std::max(v, (short)0); }
template<> inline uint64_t saturate_cast<uint64_t>(int v)        { return (uint64_t)std::max(v, (int)0); }
template<> inline uint64_t saturate_cast<uint64_t>(int64_t v)      { return (uint64_t)std::max(v, (int64_t)0); }

template<> inline int64_t saturate_cast<int64_t>(uint64_t v)       { return (int64_t)std::min(v, (uint64_t)LLONG_MAX); }
#if 0 //FP16
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(float16_t v) { return saturate_cast<_Tp>((float)v); }

// in theory, we could use a LUT for 8u/8s->16f conversion,
// but with hardware support for FP32->FP16 conversion the current approach is preferable
template<> inline float16_t saturate_cast<float16_t>(uint8_t v)   { return float16_t((float)v); }
template<> inline float16_t saturate_cast<float16_t>(int8_t v)   { return float16_t((float)v); }
template<> inline float16_t saturate_cast<float16_t>(uint16_t v)  { return float16_t((float)v); }
template<> inline float16_t saturate_cast<float16_t>(short v)   { return float16_t((float)v); }
template<> inline float16_t saturate_cast<float16_t>(unsigned v){ return float16_t((float)v); }
template<> inline float16_t saturate_cast<float16_t>(int v)     { return float16_t((float)v); }
template<> inline float16_t saturate_cast<float16_t>(uint64_t v)  { return float16_t((float)v); }
template<> inline float16_t saturate_cast<float16_t>(int64_t v)   { return float16_t((float)v); }
template<> inline float16_t saturate_cast<float16_t>(float v)   { return float16_t(v); }
template<> inline float16_t saturate_cast<float16_t>(double v)  { return float16_t((float)v); }
#endif
} //namespace saber
} //namespace anakin
#endif // ANAKIN_SABER_FUNCS_SATURATE_H
