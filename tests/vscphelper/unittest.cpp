// unittest.cpp
//
// Unit tests for vscphelper.cpp
//
// Copyright © 2000-2025 Ake Hedman, the VSCP project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <gtest/gtest.h>
#include <string>
#include <deque>
#include <cstring>
#include <climits>
#include <cmath>

#include "vscphelper.h"
#include "vscp.h"
#include "guid.h"

// =============================================================================
//                           String Value Parsing
// =============================================================================

TEST(VscpHelper, readStringValue_Decimal)
{
    EXPECT_EQ(54321, vscp_readStringValue("54321"));
    EXPECT_EQ(102, vscp_readStringValue("102"));
    EXPECT_EQ(12345, vscp_readStringValue("12345"));
    EXPECT_EQ(0, vscp_readStringValue("0"));
    EXPECT_EQ(1, vscp_readStringValue("1"));
}

TEST(VscpHelper, readStringValue_Hexadecimal)
{
    EXPECT_EQ(65535, vscp_readStringValue("0xffff"));
    EXPECT_EQ(255, vscp_readStringValue("0xFF"));
    EXPECT_EQ(16, vscp_readStringValue("0x10"));
    EXPECT_EQ(0, vscp_readStringValue("0x0"));
    EXPECT_EQ(0xABCD, vscp_readStringValue("0xABCD"));
}

TEST(VscpHelper, readStringValue_Octal)
{
    EXPECT_EQ(63, vscp_readStringValue("0o77"));
    EXPECT_EQ(8, vscp_readStringValue("0o10"));
    EXPECT_EQ(0, vscp_readStringValue("0o0"));
    EXPECT_EQ(7, vscp_readStringValue("0o7"));
}

TEST(VscpHelper, readStringValue_Binary)
{
    EXPECT_EQ(10, vscp_readStringValue("0b1010"));
    EXPECT_EQ(255, vscp_readStringValue("0b11111111"));
    EXPECT_EQ(0, vscp_readStringValue("0b0"));
    EXPECT_EQ(1, vscp_readStringValue("0b1"));
    // Invalid binary digits should stop parsing
    EXPECT_EQ(1, vscp_readStringValue("0b189"));
}

TEST(VscpHelper, readStringValue_LargeNumbers)
{
    EXPECT_EQ(UINT32_MAX, vscp_readStringValue("4294967295"));
}

// =============================================================================
//                           String Case Conversion
// =============================================================================

TEST(VscpHelper, lowercase)
{
    // vscp_lowercase returns lowercase of first character, not modifies string
    EXPECT_EQ('h', vscp_lowercase("HELLO"));
    EXPECT_EQ('a', vscp_lowercase("ABC"));
    EXPECT_EQ('z', vscp_lowercase("Z"));
    EXPECT_EQ('0', vscp_lowercase("0"));  // non-alpha unchanged
}

TEST(VscpHelper, makeUpper_InPlace)
{
    std::string s1 = "hello world";
    vscp_makeUpper(s1);
    EXPECT_EQ("HELLO WORLD", s1);
    
    std::string s2 = "HeLLo WoRLd";
    vscp_makeUpper(s2);
    EXPECT_EQ("HELLO WORLD", s2);
}

TEST(VscpHelper, makeUpper_Copy)
{
    std::string s = "hello world";
    std::string result = vscp_makeUpper_copy(s);
    EXPECT_EQ("HELLO WORLD", result);
    EXPECT_EQ("hello world", s);  // Original unchanged
}

TEST(VscpHelper, upper)
{
    std::string result = vscp_upper("hello world");
    EXPECT_EQ("HELLO WORLD", result);
}

TEST(VscpHelper, makeLower_InPlace)
{
    std::string s1 = "HELLO WORLD";
    vscp_makeLower(s1);
    EXPECT_EQ("hello world", s1);
    
    std::string s2 = "HeLLo WoRLd";
    vscp_makeLower(s2);
    EXPECT_EQ("hello world", s2);
}

TEST(VscpHelper, makeLower_Copy)
{
    std::string s = "HELLO WORLD";
    std::string result = vscp_makeLower_copy(s);
    EXPECT_EQ("hello world", result);
    EXPECT_EQ("HELLO WORLD", s);  // Original unchanged
}

TEST(VscpHelper, lower)
{
    std::string result = vscp_lower("HELLO WORLD");
    EXPECT_EQ("hello world", result);
}

// =============================================================================
//                           String Comparison
// =============================================================================

TEST(VscpHelper, strcasecmp)
{
    EXPECT_EQ(0, vscp_strcasecmp("hello", "HELLO"));
    EXPECT_EQ(0, vscp_strcasecmp("HeLLo", "hElLo"));
    EXPECT_NE(0, vscp_strcasecmp("hello", "world"));
    EXPECT_NE(0, vscp_strcasecmp("hello", "hello1"));
}

TEST(VscpHelper, strncasecmp)
{
    EXPECT_EQ(0, vscp_strncasecmp("hello", "HELLO", 5));
    EXPECT_EQ(0, vscp_strncasecmp("hello_world", "HELLO_PLANET", 5));
    EXPECT_NE(0, vscp_strncasecmp("hello_world", "HELLO_PLANET", 10));
}

// =============================================================================
//                           String Trimming
// =============================================================================

TEST(VscpHelper, ltrim_InPlace)
{
    std::string s1 = "   hello";
    vscp_ltrim(s1);
    EXPECT_EQ("hello", s1);
    
    std::string s2 = "\t\n  hello";
    vscp_ltrim(s2);
    EXPECT_EQ("hello", s2);
    
    std::string s3 = "hello   ";
    vscp_ltrim(s3);
    EXPECT_EQ("hello   ", s3);  // Right side unchanged
}

TEST(VscpHelper, rtrim_InPlace)
{
    std::string s1 = "hello   ";
    vscp_rtrim(s1);
    EXPECT_EQ("hello", s1);
    
    std::string s2 = "hello\t\n  ";
    vscp_rtrim(s2);
    EXPECT_EQ("hello", s2);
    
    std::string s3 = "   hello";
    vscp_rtrim(s3);
    EXPECT_EQ("   hello", s3);  // Left side unchanged
}

TEST(VscpHelper, trim_InPlace)
{
    std::string s1 = "   hello   ";
    vscp_trim(s1);
    EXPECT_EQ("hello", s1);
    
    std::string s2 = "\t\n  hello  \t\n";
    vscp_trim(s2);
    EXPECT_EQ("hello", s2);
}

TEST(VscpHelper, ltrim_Copy)
{
    std::string result = vscp_ltrim_copy("   hello");
    EXPECT_EQ("hello", result);
}

TEST(VscpHelper, rtrim_Copy)
{
    std::string result = vscp_rtrim_copy("hello   ");
    EXPECT_EQ("hello", result);
}

TEST(VscpHelper, trim_Copy)
{
    std::string result = vscp_trim_copy("   hello   ");
    EXPECT_EQ("hello", result);
}

TEST(VscpHelper, trimWhiteSpace)
{
    char buf[] = "   hello world   ";
    char *result = vscp_trimWhiteSpace(buf);
    EXPECT_STREQ("hello world", result);
}

// =============================================================================
//                           String Split
// =============================================================================

TEST(VscpHelper, split_Comma)
{
    std::deque<std::string> parts;
    vscp_split(parts, "one,two,three", ",");
    ASSERT_EQ(3u, parts.size());
    EXPECT_EQ("one", parts[0]);
    EXPECT_EQ("two", parts[1]);
    EXPECT_EQ("three", parts[2]);
}

TEST(VscpHelper, split_Space)
{
    std::deque<std::string> parts;
    vscp_split(parts, "one two three", " ");
    ASSERT_EQ(3u, parts.size());
    EXPECT_EQ("one", parts[0]);
    EXPECT_EQ("two", parts[1]);
    EXPECT_EQ("three", parts[2]);
}

TEST(VscpHelper, split_MultiCharDelimiter)
{
    std::deque<std::string> parts;
    vscp_split(parts, "one::two::three", "::");
    ASSERT_EQ(3u, parts.size());
    EXPECT_EQ("one", parts[0]);
    EXPECT_EQ("two", parts[1]);
    EXPECT_EQ("three", parts[2]);
}

TEST(VscpHelper, split_EmptyParts)
{
    std::deque<std::string> parts;
    vscp_split(parts, "one,,three", ",");
    ASSERT_EQ(3u, parts.size());
    EXPECT_EQ("one", parts[0]);
    EXPECT_EQ("", parts[1]);
    EXPECT_EQ("three", parts[2]);
}

// =============================================================================
//                           String Left/Right
// =============================================================================

TEST(VscpHelper, str_left)
{
    EXPECT_EQ("hel", vscp_str_left("hello", 3));
    EXPECT_EQ("hello", vscp_str_left("hello", 5));
    EXPECT_EQ("hello", vscp_str_left("hello", 10));
    EXPECT_EQ("", vscp_str_left("hello", 0));
}

TEST(VscpHelper, str_right)
{
    EXPECT_EQ("llo", vscp_str_right("hello", 3));
    EXPECT_EQ("hello", vscp_str_right("hello", 5));
    EXPECT_EQ("hello", vscp_str_right("hello", 10));
    EXPECT_EQ("", vscp_str_right("hello", 0));
}

TEST(VscpHelper, str_before)
{
    EXPECT_EQ("hello", vscp_str_before("hello,world", ','));
    EXPECT_EQ("", vscp_str_before("hello", ','));
}

TEST(VscpHelper, str_after)
{
    EXPECT_EQ("world", vscp_str_after("hello,world", ','));
    EXPECT_EQ("", vscp_str_after("hello", ','));
}

// =============================================================================
//                           String Starts With
// =============================================================================

TEST(VscpHelper, startsWith)
{
    EXPECT_TRUE(vscp_startsWith("hello world", "hello"));
    EXPECT_TRUE(vscp_startsWith("hello world", "h"));
    EXPECT_TRUE(vscp_startsWith("hello world", "hello world"));
    EXPECT_FALSE(vscp_startsWith("hello world", "world"));
    EXPECT_FALSE(vscp_startsWith("hello", "hello world"));
    EXPECT_TRUE(vscp_startsWith("hello", ""));
}

// =============================================================================
//                           String Reverse
// =============================================================================

TEST(VscpHelper, reverse)
{
    // vscp_reverse returns a newly allocated reversed string
    char *r1 = vscp_reverse("hello");
    ASSERT_NE(nullptr, r1);
    EXPECT_STREQ("olleh", r1);
    free(r1);
    
    char *r2 = vscp_reverse("ab");
    ASSERT_NE(nullptr, r2);
    EXPECT_STREQ("ba", r2);
    free(r2);
    
    char *r3 = vscp_reverse("a");
    ASSERT_NE(nullptr, r3);
    EXPECT_STREQ("a", r3);
    free(r3);
}

// =============================================================================
//                           String Format
// =============================================================================

TEST(VscpHelper, str_format)
{
    EXPECT_EQ("Hello World", vscp_str_format("Hello %s", "World"));
    EXPECT_EQ("Number: 42", vscp_str_format("Number: %d", 42));
    EXPECT_EQ("Hex: ff", vscp_str_format("Hex: %x", 255));
}

// =============================================================================
//                           String Search
// =============================================================================

TEST(VscpHelper, strcasestr)
{
    const char *str = "Hello World";
    EXPECT_NE(nullptr, vscp_strcasestr(str, "world"));
    EXPECT_NE(nullptr, vscp_strcasestr(str, "HELLO"));
    EXPECT_EQ(nullptr, vscp_strcasestr(str, "xyz"));
}

TEST(VscpHelper, stristr)
{
    char str[] = "Hello World";
    char *result = vscp_stristr(str, "world");
    EXPECT_NE(nullptr, result);
    if (result) {
        EXPECT_EQ(str + 6, result);
    }
}

// =============================================================================
//                           is_Number
// =============================================================================

TEST(VscpHelper, isNumber)
{
    EXPECT_TRUE(vscp_isNumber("123"));
    EXPECT_TRUE(vscp_isNumber("0x1F"));
    EXPECT_TRUE(vscp_isNumber("0o77"));
    EXPECT_TRUE(vscp_isNumber("0b1010"));
    EXPECT_TRUE(vscp_isNumber("  123  "));  // With whitespace
    EXPECT_FALSE(vscp_isNumber("abc"));
    EXPECT_FALSE(vscp_isNumber("hello123"));
}

// =============================================================================
//                           Float Comparison
// =============================================================================

TEST(VscpHelper, almostEqualRelativeFloat)
{
    EXPECT_TRUE(vscp_almostEqualRelativeFloat(1.0f, 1.0f, 0.001f));
    EXPECT_TRUE(vscp_almostEqualRelativeFloat(1.0f, 1.0001f, 0.001f));
    EXPECT_FALSE(vscp_almostEqualRelativeFloat(1.0f, 1.01f, 0.001f));
    EXPECT_TRUE(vscp_almostEqualRelativeFloat(0.0f, 0.0f, 0.001f));
}

TEST(VscpHelper, almostEqualRelativeDouble)
{
    EXPECT_TRUE(vscp_almostEqualRelativeDouble(1.0, 1.0, 0.001));
    EXPECT_TRUE(vscp_almostEqualRelativeDouble(1.0, 1.0001, 0.001));
    EXPECT_FALSE(vscp_almostEqualRelativeDouble(1.0, 1.01, 0.001));
    EXPECT_TRUE(vscp_almostEqualRelativeDouble(0.0, 0.0, 0.001));
}

// =============================================================================
//                           Platform Detection
// =============================================================================

TEST(VscpHelper, platformBitness)
{
    // One of these must be true
    EXPECT_TRUE(vscp_is64Bit() || vscp_is32Bit());
    // They should be mutually exclusive
    EXPECT_NE(vscp_is64Bit(), vscp_is32Bit());
}

TEST(VscpHelper, platformEndianness)
{
    // One of these must be true
    EXPECT_TRUE(vscp_isLittleEndian() || vscp_isBigEndian());
    // They should be mutually exclusive
    EXPECT_NE(vscp_isLittleEndian(), vscp_isBigEndian());
}

// =============================================================================
//                           strlcpy / strdup
// =============================================================================

TEST(VscpHelper, strlcpy)
{
    char dest[10];
    vscp_strlcpy(dest, "hello", sizeof(dest));
    EXPECT_STREQ("hello", dest);
    
    // Test truncation
    vscp_strlcpy(dest, "hello world!", sizeof(dest));
    EXPECT_STREQ("hello wor", dest);  // Truncated to fit buffer
}

TEST(VscpHelper, strdup)
{
    char *dup = vscp_strdup("hello");
    ASSERT_NE(nullptr, dup);
    EXPECT_STREQ("hello", dup);
    free(dup);
}

TEST(VscpHelper, strndup)
{
    char *dup = vscp_strndup("hello world", 5);
    ASSERT_NE(nullptr, dup);
    EXPECT_STREQ("hello", dup);
    free(dup);
}

// =============================================================================
//                           Base64 Encoding/Decoding
// =============================================================================

TEST(VscpHelper, base64_encode_decode)
{
    std::string original = "Hello, World!";
    std::string encoded = original;
    
    EXPECT_TRUE(vscp_base64_std_encode(encoded));
    EXPECT_NE(original, encoded);
    
    EXPECT_TRUE(vscp_base64_std_decode(encoded));
    EXPECT_EQ(original, encoded);
}

TEST(VscpHelper, base64_convertToBase64)
{
    std::string result = vscp_convertToBase64("Hello");
    EXPECT_FALSE(result.empty());
    
    // Decode it back
    EXPECT_TRUE(vscp_base64_std_decode(result));
    EXPECT_EQ("Hello", result);
}

TEST(VscpHelper, decodeBase64IfNeeded)
{
    // With BASE64: prefix
    std::string encoded;
    vscp_base64_std_encode(encoded = "Hello");
    std::string withPrefix = "BASE64:" + encoded;
    std::string result;
    EXPECT_TRUE(vscp_std_decodeBase64IfNeeded(withPrefix, result));
    EXPECT_EQ("Hello", result);
    
    // Without prefix - should pass through unchanged
    std::string plain = "Hello";
    EXPECT_TRUE(vscp_std_decodeBase64IfNeeded(plain, result));
    EXPECT_EQ("Hello", result);
}

// =============================================================================
//                           Time Functions
// =============================================================================

TEST(VscpHelper, to_unix_ns)
{
    // Test Jan 1, 2000, 00:00:00
    int64_t ns = vscp_to_unix_ns(2000, 1, 1, 0, 0, 0, 0);
    EXPECT_GT(ns, 0);  // Should be positive
    
    // Unix epoch (1970-01-01)
    ns = vscp_to_unix_ns(1970, 1, 1, 0, 0, 0, 0);
    EXPECT_EQ(0, ns);
}

TEST(VscpHelper, from_unix_ns)
{
    int year, month, day, hour, minute, second;
    uint32_t timestamp;
    
    // Unix epoch
    vscp_from_unix_ns(0, &year, &month, &day, &hour, &minute, &second, &timestamp);
    EXPECT_EQ(1970, year);
    EXPECT_EQ(1, month);
    EXPECT_EQ(1, day);
    EXPECT_EQ(0, hour);
    EXPECT_EQ(0, minute);
    EXPECT_EQ(0, second);
}

TEST(VscpHelper, to_from_unix_ns_roundtrip)
{
    // Create a timestamp
    int64_t original_ns = vscp_to_unix_ns(2024, 6, 15, 12, 30, 45, 123456);
    
    // Convert back
    int year, month, day, hour, minute, second;
    uint32_t timestamp;
    vscp_from_unix_ns(original_ns, &year, &month, &day, &hour, &minute, &second, &timestamp);
    
    EXPECT_EQ(2024, year);
    EXPECT_EQ(6, month);
    EXPECT_EQ(15, day);
    EXPECT_EQ(12, hour);
    EXPECT_EQ(30, minute);
    EXPECT_EQ(45, second);
}

// =============================================================================
//                           GUID Functions
// =============================================================================

TEST(VscpHelper, getGuidFromStringToArray)
{
    uint8_t guid[16];
    std::string strGUID = "00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F";
    
    EXPECT_TRUE(vscp_getGuidFromStringToArray(guid, strGUID));
    
    for (int i = 0; i < 16; i++) {
        EXPECT_EQ(i, guid[i]);
    }
}

TEST(VscpHelper, writeGuidArrayToString)
{
    uint8_t guid[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    std::string strGUID;
    
    EXPECT_TRUE(vscp_writeGuidArrayToString(strGUID, guid));
    EXPECT_EQ("00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F", strGUID);
}

TEST(VscpHelper, isGUIDEmpty)
{
    uint8_t emptyGuid[16] = {0};
    uint8_t nonEmptyGuid[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    
    EXPECT_TRUE(vscp_isGUIDEmpty(emptyGuid));
    EXPECT_FALSE(vscp_isGUIDEmpty(nonEmptyGuid));
}

TEST(VscpHelper, isSameGUID)
{
    uint8_t guid1[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t guid2[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t guid3[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0};
    
    EXPECT_TRUE(vscp_isSameGUID(guid1, guid2));
    EXPECT_FALSE(vscp_isSameGUID(guid1, guid3));
}

TEST(VscpHelper, reverseGUID)
{
    uint8_t guid[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    
    EXPECT_TRUE(vscp_reverseGUID(guid));
    
    EXPECT_EQ(15, guid[0]);
    EXPECT_EQ(14, guid[1]);
    EXPECT_EQ(0, guid[15]);
}

TEST(VscpHelper, calcCRC4GUIDArray)
{
    uint8_t guid[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    
    uint8_t crc = vscp_calcCRC4GUIDArray(guid);
    // CRC should be consistent for same input
    EXPECT_EQ(crc, vscp_calcCRC4GUIDArray(guid));
}

TEST(VscpHelper, calcCRC4GUIDString)
{
    std::string strGUID = "FF:FF:FF:FF:FF:FF:FF:FE:00:00:00:00:00:00:00:01";
    
    uint8_t crc = vscp_calcCRC4GUIDString(strGUID);
    // CRC should be consistent for same input
    EXPECT_EQ(crc, vscp_calcCRC4GUIDString(strGUID));
}

// =============================================================================
//                           Event Basic Operations
// =============================================================================

TEST(VscpHelper, newEvent)
{
    vscpEvent *pEvent = nullptr;
    EXPECT_TRUE(vscp_newEvent(&pEvent));
    ASSERT_NE(nullptr, pEvent);
    
    vscp_deleteEvent(pEvent);
}

TEST(VscpHelper, makeTimeStamp)
{
    unsigned long ts1 = vscp_makeTimeStamp();
    unsigned long ts2 = vscp_makeTimeStamp();
    
    // Timestamps should be non-zero and close in value
    EXPECT_GT(ts1, 0u);
    EXPECT_GE(ts2, ts1);
}

TEST(VscpHelper, getMsTimeStamp)
{
    uint32_t ts1 = vscp_getMsTimeStamp();
    uint32_t ts2 = vscp_getMsTimeStamp();
    
    EXPECT_GE(ts2, ts1);
}

// =============================================================================
//                           Event Priority
// =============================================================================

TEST(VscpHelper, setGetEventPriority)
{
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    
    for (unsigned char p = 0; p < 8; p++) {
        vscp_setEventPriority(&event, p);
        EXPECT_EQ(p, vscp_getEventPriority(&event));
    }
}

TEST(VscpHelper, setGetEventExPriority)
{
    vscpEventEx eventEx;
    memset(&eventEx, 0, sizeof(eventEx));
    
    for (unsigned char p = 0; p < 8; p++) {
        vscp_setEventExPriority(&eventEx, p);
        EXPECT_EQ(p, vscp_getEventExPriority(&eventEx));
    }
}

// =============================================================================
//                         Event Conversion
// =============================================================================

TEST(VscpHelper, eventToEventEx_Empty)
{
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.pdata = nullptr;
    event.sizeData = 0;
    event.vscp_class = 10;
    event.vscp_type = 6;
    
    vscpEventEx eventEx;
    EXPECT_TRUE(vscp_convertEventToEventEx(&eventEx, &event));
    EXPECT_EQ(10, eventEx.vscp_class);
    EXPECT_EQ(6, eventEx.vscp_type);
    EXPECT_EQ(0, eventEx.sizeData);
}

TEST(VscpHelper, eventExToEvent)
{
    vscpEventEx eventEx;
    memset(&eventEx, 0, sizeof(eventEx));
    eventEx.vscp_class = 20;
    eventEx.vscp_type = 3;
    eventEx.sizeData = 3;
    eventEx.data[0] = 1;
    eventEx.data[1] = 2;
    eventEx.data[2] = 3;
    
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.pdata = nullptr;
    
    EXPECT_TRUE(vscp_convertEventExToEvent(&event, &eventEx));
    EXPECT_EQ(20, event.vscp_class);
    EXPECT_EQ(3, event.vscp_type);
    EXPECT_EQ(3, event.sizeData);
    ASSERT_NE(nullptr, event.pdata);
    EXPECT_EQ(1, event.pdata[0]);
    EXPECT_EQ(2, event.pdata[1]);
    EXPECT_EQ(3, event.pdata[2]);
    
    vscp_deleteEvent(&event);
}

TEST(VscpHelper, copyEvent)
{
    vscpEvent eventFrom;
    memset(&eventFrom, 0, sizeof(eventFrom));
    eventFrom.vscp_class = 10;
    eventFrom.vscp_type = 6;
    eventFrom.sizeData = 3;
    eventFrom.pdata = new uint8_t[3]{1, 2, 3};
    
    vscpEvent eventTo;
    memset(&eventTo, 0, sizeof(eventTo));
    eventTo.pdata = nullptr;
    
    EXPECT_TRUE(vscp_copyEvent(&eventTo, &eventFrom));
    EXPECT_EQ(10, eventTo.vscp_class);
    EXPECT_EQ(6, eventTo.vscp_type);
    EXPECT_EQ(3, eventTo.sizeData);
    ASSERT_NE(nullptr, eventTo.pdata);
    EXPECT_EQ(1, eventTo.pdata[0]);
    EXPECT_EQ(2, eventTo.pdata[1]);
    EXPECT_EQ(3, eventTo.pdata[2]);
    
    delete[] eventFrom.pdata;
    delete[] eventTo.pdata;
}

TEST(VscpHelper, copyEventEx)
{
    vscpEventEx eventFrom;
    memset(&eventFrom, 0, sizeof(eventFrom));
    eventFrom.vscp_class = 10;
    eventFrom.vscp_type = 6;
    eventFrom.sizeData = 3;
    eventFrom.data[0] = 1;
    eventFrom.data[1] = 2;
    eventFrom.data[2] = 3;
    
    vscpEventEx eventTo;
    EXPECT_TRUE(vscp_copyEventEx(&eventTo, &eventFrom));
    EXPECT_EQ(10, eventTo.vscp_class);
    EXPECT_EQ(6, eventTo.vscp_type);
    EXPECT_EQ(3, eventTo.sizeData);
    EXPECT_EQ(1, eventTo.data[0]);
    EXPECT_EQ(2, eventTo.data[1]);
    EXPECT_EQ(3, eventTo.data[2]);
}

// =============================================================================
//                         Event String Conversion
// =============================================================================

TEST(VscpHelper, convertEventToString)
{
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.head = 0;
    event.vscp_class = 10;
    event.vscp_type = 6;
    event.obid = 0;
    event.timestamp = 0;
    memset(event.GUID, 0, 16);
    event.sizeData = 3;
    event.pdata = new uint8_t[3]{0x01, 0x02, 0x03};
    
    std::string str;
    EXPECT_TRUE(vscp_convertEventToString(str, &event));
    EXPECT_FALSE(str.empty());
    
    delete[] event.pdata;
}

TEST(VscpHelper, convertStringToEvent)
{
    // Format: head,class,type,obid,datetime,timestamp,GUID,data1,data2,data3....
    std::string str = "0,10,6,0,,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00,0x01,0x02,0x03";
    
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.pdata = nullptr;
    
    EXPECT_TRUE(vscp_convertStringToEvent(&event, str));
    EXPECT_EQ(10, event.vscp_class);
    EXPECT_EQ(6, event.vscp_type);
    EXPECT_EQ(3, event.sizeData);
    ASSERT_NE(nullptr, event.pdata);
    EXPECT_EQ(1, event.pdata[0]);
    EXPECT_EQ(2, event.pdata[1]);
    EXPECT_EQ(3, event.pdata[2]);
    
    vscp_deleteEvent(&event);
}

TEST(VscpHelper, convertEventExToString)
{
    vscpEventEx eventEx;
    memset(&eventEx, 0, sizeof(eventEx));
    eventEx.vscp_class = 10;
    eventEx.vscp_type = 6;
    eventEx.sizeData = 3;
    eventEx.data[0] = 1;
    eventEx.data[1] = 2;
    eventEx.data[2] = 3;
    
    std::string str;
    EXPECT_TRUE(vscp_convertEventExToString(str, &eventEx));
    EXPECT_FALSE(str.empty());
}

TEST(VscpHelper, convertStringToEventEx)
{
    // Format: head,class,type,obid,datetime,timestamp,GUID,data1,data2,data3....
    std::string str = "0,10,6,0,,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00,0x01,0x02,0x03";
    
    vscpEventEx eventEx;
    memset(&eventEx, 0, sizeof(eventEx));
    
    EXPECT_TRUE(vscp_convertStringToEventEx(&eventEx, str));
    EXPECT_EQ(10, eventEx.vscp_class);
    EXPECT_EQ(6, eventEx.vscp_type);
    EXPECT_EQ(3, eventEx.sizeData);
    EXPECT_EQ(1, eventEx.data[0]);
    EXPECT_EQ(2, eventEx.data[1]);
    EXPECT_EQ(3, eventEx.data[2]);
}

// =============================================================================
//                         Event Data String Conversion
// =============================================================================

TEST(VscpHelper, setEventDataFromString)
{
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.pdata = nullptr;
    
    EXPECT_TRUE(vscp_setEventDataFromString(&event, "0x01,0x02,0x03,0x04"));
    EXPECT_EQ(4, event.sizeData);
    ASSERT_NE(nullptr, event.pdata);
    EXPECT_EQ(1, event.pdata[0]);
    EXPECT_EQ(2, event.pdata[1]);
    EXPECT_EQ(3, event.pdata[2]);
    EXPECT_EQ(4, event.pdata[3]);
    
    vscp_deleteEvent(&event);
}

TEST(VscpHelper, setEventExDataFromString)
{
    vscpEventEx eventEx;
    memset(&eventEx, 0, sizeof(eventEx));
    
    EXPECT_TRUE(vscp_setEventExDataFromString(&eventEx, "0x01,0x02,0x03,0x04"));
    EXPECT_EQ(4, eventEx.sizeData);
    EXPECT_EQ(1, eventEx.data[0]);
    EXPECT_EQ(2, eventEx.data[1]);
    EXPECT_EQ(3, eventEx.data[2]);
    EXPECT_EQ(4, eventEx.data[3]);
}

TEST(VscpHelper, writeDataWithSizeToString)
{
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    std::string str;
    
    EXPECT_TRUE(vscp_writeDataWithSizeToString(str, data, sizeof(data)));
    EXPECT_FALSE(str.empty());
}

// =============================================================================
//                         Filter Operations
// =============================================================================

TEST(VscpHelper, clearVSCPFilter)
{
    vscpEventFilter filter;
    filter.filter_priority = 7;
    filter.filter_class = 100;
    filter.filter_type = 50;
    filter.mask_priority = 7;
    filter.mask_class = 0xFFFF;
    filter.mask_type = 0xFFFF;
    
    vscp_clearVSCPFilter(&filter);
    
    EXPECT_EQ(0, filter.filter_priority);
    EXPECT_EQ(0, filter.filter_class);
    EXPECT_EQ(0, filter.filter_type);
    EXPECT_EQ(0, filter.mask_priority);
    EXPECT_EQ(0, filter.mask_class);
    EXPECT_EQ(0, filter.mask_type);
}

TEST(VscpHelper, copyVSCPFilter)
{
    vscpEventFilter filterFrom;
    filterFrom.filter_priority = 7;
    filterFrom.filter_class = 100;
    filterFrom.filter_type = 50;
    filterFrom.mask_priority = 7;
    filterFrom.mask_class = 0xFFFF;
    filterFrom.mask_type = 0xFFFF;
    memset(filterFrom.filter_GUID, 0xFF, 16);
    memset(filterFrom.mask_GUID, 0xFF, 16);
    
    vscpEventFilter filterTo;
    vscp_copyVSCPFilter(&filterTo, &filterFrom);
    
    EXPECT_EQ(filterFrom.filter_priority, filterTo.filter_priority);
    EXPECT_EQ(filterFrom.filter_class, filterTo.filter_class);
    EXPECT_EQ(filterFrom.filter_type, filterTo.filter_type);
    EXPECT_EQ(filterFrom.mask_priority, filterTo.mask_priority);
    EXPECT_EQ(filterFrom.mask_class, filterTo.mask_class);
    EXPECT_EQ(filterFrom.mask_type, filterTo.mask_type);
}

TEST(VscpHelper, doLevel2Filter_NullFilter)
{
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.pdata = nullptr;
    
    // Null filter should pass everything
    EXPECT_TRUE(vscp_doLevel2Filter(&event, nullptr));
}

TEST(VscpHelper, doLevel2Filter_EmptyMask)
{
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.pdata = nullptr;
    event.vscp_class = 100;
    event.vscp_type = 50;
    
    vscpEventFilter filter;
    vscp_clearVSCPFilter(&filter);
    
    // Empty mask (all zeros) should pass everything
    EXPECT_TRUE(vscp_doLevel2Filter(&event, &filter));
}

TEST(VscpHelper, doLevel2Filter_ClassFilter)
{
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.pdata = nullptr;
    event.vscp_class = 10;
    event.vscp_type = 6;
    
    vscpEventFilter filter;
    vscp_clearVSCPFilter(&filter);
    filter.filter_class = 10;
    filter.mask_class = 0xFFFF;
    
    EXPECT_TRUE(vscp_doLevel2Filter(&event, &filter));
    
    // Change event class, should fail
    event.vscp_class = 20;
    EXPECT_FALSE(vscp_doLevel2Filter(&event, &filter));
}

TEST(VscpHelper, readFilterFromString)
{
    vscpEventFilter filter;
    
    EXPECT_TRUE(vscp_readFilterFromString(&filter, "3,10,6,FF:FF:FF:FF:FF:FF:FF:FE:00:00:00:00:00:00:00:01"));
    EXPECT_EQ(3, filter.filter_priority);
    EXPECT_EQ(10, filter.filter_class);
    EXPECT_EQ(6, filter.filter_type);
}

TEST(VscpHelper, readMaskFromString)
{
    vscpEventFilter filter;
    
    EXPECT_TRUE(vscp_readMaskFromString(&filter, "7,0xFFFF,0xFFFF,FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF"));
    EXPECT_EQ(7, filter.mask_priority);
    EXPECT_EQ(0xFFFF, filter.mask_class);
    EXPECT_EQ(0xFFFF, filter.mask_type);
}

// =============================================================================
//                         CANAL ID Functions
// =============================================================================

TEST(VscpHelper, getHeadFromCANALid)
{
    uint32_t id = 0x1C000000;  // Priority 7 (high 3 bits)
    EXPECT_EQ(7, (vscp_getHeadFromCANALid(id) >> 5) & 0x07);
}

TEST(VscpHelper, getVscpClassFromCANALid)
{
    // Class is in bits 16-24 with 9-bit mask (0x1ff)
    // Example: class 0x0A (10) at position 16 = 0x000A0000
    uint32_t id = 0x000A0000;  // Class 10
    uint16_t vscp_class = vscp_getVscpClassFromCANALid(id);
    EXPECT_EQ(10, vscp_class);
    
    // Max class value (511)
    id = 0x01FF0000;
    vscp_class = vscp_getVscpClassFromCANALid(id);
    EXPECT_EQ(0x1FF, vscp_class);
}

TEST(VscpHelper, getVscpTypeFromCANALid)
{
    uint32_t id = 0x0000FF00;  // Type in bits 8-15
    uint16_t vscp_type = vscp_getVscpTypeFromCANALid(id);
    EXPECT_EQ(0xFF, vscp_type);
}

TEST(VscpHelper, getNicknameFromCANALid)
{
    uint32_t id = 0x000000FF;  // Nickname in bits 0-7
    uint8_t nickname = vscp_getNicknameFromCANALid(id);
    EXPECT_EQ(0xFF, nickname);
}

TEST(VscpHelper, getCANALidFromData)
{
    uint32_t id = vscp_getCANALidFromData(7, 10, 6);
    // Should have priority, class, type encoded
    EXPECT_NE(0u, id);
    
    // Verify we can extract them back
    EXPECT_EQ(10, vscp_getVscpClassFromCANALid(id));
    EXPECT_EQ(6, vscp_getVscpTypeFromCANALid(id));
}

// =============================================================================
//                         Interface Parsing
// =============================================================================

TEST(VscpHelper, getPortFromInterface)
{
    EXPECT_EQ(9598, vscp_getPortFromInterface("tcp://localhost:9598"));
    EXPECT_EQ(80, vscp_getPortFromInterface("http://example.com:80"));
    // Returns 0 instead of -1 when port is not specified
    EXPECT_EQ(0, vscp_getPortFromInterface("tcp://localhost"));
}

TEST(VscpHelper, getHostFromInterface)
{
    EXPECT_EQ("localhost", vscp_getHostFromInterface("tcp://localhost:9598"));
    EXPECT_EQ("example.com", vscp_getHostFromInterface("http://example.com:80"));
    EXPECT_EQ("localhost", vscp_getHostFromInterface("tcp://localhost"));
}

// Note: vscp_getProtocolFromInterface is declared but not implemented

// =============================================================================
//                         Safe String Encoding
// =============================================================================

TEST(VscpHelper, safe_encode_str)
{
    std::string str = "Hello\nWorld\t\"Test\"";
    std::string encoded = vscp_safe_encode_str(str);
    
    // Should encode control characters as \x followed by hex
    EXPECT_NE(str, encoded);
    // \n (0x0A) becomes \xa
    EXPECT_NE(std::string::npos, encoded.find("\\xa"));
    // \t (0x09) becomes \x9
    EXPECT_NE(std::string::npos, encoded.find("\\x9"));
}

// =============================================================================
//                         Replace Backslash
// =============================================================================

TEST(VscpHelper, replaceBackslash)
{
    std::string path = "C:\\Users\\Test\\file.txt";
    vscp_replaceBackslash(path);
    EXPECT_EQ("C:/Users/Test/file.txt", path);
    
    std::string noBackslash = "C:/Users/Test";
    vscp_replaceBackslash(noBackslash);
    EXPECT_EQ("C:/Users/Test", noBackslash);
}

// =============================================================================
//                         CRC Functions
// =============================================================================

TEST(VscpHelper, calc_crc_Event)
{
    vscpEvent event;
    memset(&event, 0, sizeof(event));
    event.vscp_class = 10;
    event.vscp_type = 6;
    event.sizeData = 3;
    event.pdata = new uint8_t[3]{1, 2, 3};
    
    unsigned short crc1 = vscp_calc_crc_Event(&event, 0);  // Don't set
    unsigned short crc2 = vscp_calc_crc_Event(&event, 0);
    
    // CRC should be consistent
    EXPECT_EQ(crc1, crc2);
    
    // Set CRC in event
    vscp_calc_crc_Event(&event, 1);
    EXPECT_EQ(crc1, event.crc);
    
    delete[] event.pdata;
}

TEST(VscpHelper, calc_crc_EventEx)
{
    vscpEventEx eventEx;
    memset(&eventEx, 0, sizeof(eventEx));
    eventEx.vscp_class = 10;
    eventEx.vscp_type = 6;
    eventEx.sizeData = 3;
    eventEx.data[0] = 1;
    eventEx.data[1] = 2;
    eventEx.data[2] = 3;
    
    unsigned short crc1 = vscp_calc_crc_EventEx(&eventEx, 0);
    unsigned short crc2 = vscp_calc_crc_EventEx(&eventEx, 0);
    
    // CRC should be consistent
    EXPECT_EQ(crc1, crc2);
}

// Entry point for Google Test
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
