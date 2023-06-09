//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_patches_hpp
#define kern_patches_hpp
#include <Headers/kern_util.hpp>

/**
 * `AMDSupport`
 *  Neutrallises VRAM Info Null check
 */
static const uint8_t kVRAMInfoNullCheckOriginal[] = {0x48, 0x89, 0x83, 0x18, 0x01, 0x00, 0x00, 0x31, 0xc0, 0x48, 0x85,
    0xc9, 0x75, 0x3e, 0x48, 0x8d, 0x3d, 0xa4, 0xe2, 0x01, 0x00};
static const uint8_t kVRAMInfoNullCheckPatched[] = {0x48, 0x89, 0x83, 0x18, 0x01, 0x00, 0x00, 0x31, 0xc0, 0x48, 0x85,
    0xc9, 0x74, 0x3e, 0x48, 0x8d, 0x3d, 0xa4, 0xe2, 0x01, 0x00};

/**
 * `AppleGraphicsDevicePolicy`
 * Symbols are stripped so function is unknown.
 * Removes framebuffer count >= 2 check.
 */
static const uint8_t kAGDPFBCountCheckOriginal[] = {0x02, 0x00, 0x00, 0x83, 0xF8, 0x02};
static const uint8_t kAGDPFBCountCheckPatched[] = {0x02, 0x00, 0x00, 0x83, 0xF8, 0x00};

/**
 * `AppleGraphicsDevicePolicy::start`
 * Neutralise access to AGDP configuration by board identifier.
 */
static const char kAGDPBoardIDKeyOriginal[] = "board-id";
static const char kAGDPBoardIDKeyPatched[] = "applehax";

/**
 * `AMDRadeonX4000_AMDHardware::startHWEngines`
 * Make for loop run only once as we only have one SDMA engine.
 * Patch originally came from NootedRed, since the code for startHWEngines is nearly identical on X4000, this patch, in
 * theory, should work
 */
static const uint8_t kStartHWEnginesOriginal[] = {0x40, 0x83, 0xF0, 0x02};
static const uint8_t kStartHWEnginesMask[] = {0xF0, 0xFF, 0xF0, 0xFF};
static const uint8_t kStartHWEnginesPatched[] = {0x40, 0x83, 0xF0, 0x01};

/** VideoToolbox DRM model check */
static const char kVideoToolboxDRMModelOriginal[] = "MacPro5,1\0MacPro6,1\0IOService";

static const uint8_t kBoardIdOriginal[] = {0x62, 0x6F, 0x61, 0x72, 0x64, 0x2D, 0x69, 0x64, 0x00, 0x68, 0x77, 0x2E, 0x6D,
    0x6F, 0x64, 0x65, 0x6C};
static const uint8_t kBoardIdPatched[] = {0x68, 0x77, 0x67, 0x76, 0x61};

static const char kCoreLSKDMSEPath[] = "/System/Library/PrivateFrameworks/CoreLSKDMSE.framework/Versions/A/CoreLSKDMSE";
static const char kCoreLSKDPath[] = "/System/Library/PrivateFrameworks/CoreLSKD.framework/Versions/A/CoreLSKD";

static const uint8_t kCoreLSKDOriginal[] = {0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x0F, 0xA2};
static const uint8_t kCoreLSKDPatched[] = {0xC7, 0xC0, 0xC3, 0x06, 0x03, 0x00, 0x90, 0x90};

static_assert(arrsize(kVRAMInfoNullCheckOriginal) == arrsize(kVRAMInfoNullCheckPatched));
static_assert(arrsize(kAGDPFBCountCheckOriginal) == arrsize(kAGDPFBCountCheckPatched));
static_assert(arrsize(kAGDPBoardIDKeyOriginal) == arrsize(kAGDPBoardIDKeyPatched));
static_assert(arrsize(kStartHWEnginesOriginal) == arrsize(kStartHWEnginesPatched));
static_assert(arrsize(kCoreLSKDOriginal) == arrsize(kCoreLSKDPatched));

#endif /* kern_patches_hpp */
