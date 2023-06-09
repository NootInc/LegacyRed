//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_gfxcon.hpp"
#include "kern_lred.hpp"
#include "kern_patches.hpp"
#include <Headers/kern_api.hpp>

static const char *pathRadeonGFX7Con =
    "/System/Library/Extensions/AMD8000Controller.kext/Contents/MacOS/AMD8000Controller";

static const char *pathRadeonGFX8Con =
    "/System/Library/Extensions/AMD9000Controller.kext/Contents/MacOS/AMD9000Controller";

static const char *pathRadeonPolarisCon =
    "/System/Library/Extensions/AMD9500Controller.kext/Contents/MacOS/AMD9500Controller";

static KernelPatcher::KextInfo kextRadeonGFX7Con = {"com.apple.kext.AMD8000Controller", &pathRadeonGFX7Con, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};

static KernelPatcher::KextInfo kextRadeonGFX8Con = {"com.apple.kext.AMD9000Controller", &pathRadeonGFX8Con, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};

static KernelPatcher::KextInfo kextRadeonPolarisCon = {"com.apple.kext.AMD9500Controller", &pathRadeonPolarisCon, 1, {},
    {}, KernelPatcher::KextInfo::Unloaded};

GFXCon *GFXCon::callback = nullptr;

void GFXCon::init() {
    callback = this;
    lilu.onKextLoadForce(&kextRadeonGFX7Con);
    lilu.onKextLoadForce(&kextRadeonGFX8Con);
    lilu.onKextLoadForce(&kextRadeonPolarisCon);
}

bool GFXCon::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (kextRadeonGFX7Con.loadIndex == index) {
        LRed::callback->setRMMIOIfNecessary();
        auto highsierra = getKernelVersion() == KernelVersion::HighSierra;
        auto regDbg = checkKernelArgument("-lredregdbg");

        RouteRequestPlus requests[] = {
            {"__ZN18CISharedController11getFamilyIdEv", wrapGetFamilyId, orgGetFamilyId, highsierra},
            {"__ZNK18CISharedController11getFamilyIdEv", wrapGetFamilyId, orgGetFamilyId, !highsierra},
            {"__ZN17CIRegisterService10hwReadReg8Ej", wrapHwReadReg8, this->orgHwReadReg8, regDbg},
            {"__ZN17CIRegisterService11hwReadReg16Ej", wrapHwReadReg16, this->orgHwReadReg16, regDbg},
            {"__ZN17CIRegisterService11hwReadReg32Ej", wrapHwReadReg32, this->orgHwReadReg32, regDbg},
            {"__ZN13ASIC_INFO__CI18populateDeviceInfoEv", wrapPopulateDeviceInfo, this->orgPopulateDeviceInfo,
                !highsierra},
        };
        PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "gfxcon",
            "Failed to route symbols");

        return true;
    } else if (kextRadeonGFX8Con.loadIndex == index) {
        LRed::callback->setRMMIOIfNecessary();
        auto highsierra = getKernelVersion() == KernelVersion::HighSierra;
        auto regDbg = checkKernelArgument("-lredregdbg");

        RouteRequestPlus requests[] = {
            {"__ZN18VISharedController11getFamilyIdEv", wrapGetFamilyId, orgGetFamilyId, highsierra},
            {"__ZNK18VISharedController11getFamilyIdEv", wrapGetFamilyId, orgGetFamilyId, !highsierra},
            {"__ZN17VIRegisterService10hwReadReg8Ej", wrapHwReadReg8, this->orgHwReadReg8, regDbg},
            {"__ZN17VIRegisterService11hwReadReg16Ej", wrapHwReadReg16, this->orgHwReadReg16, regDbg},
            {"__ZN17VIRegisterService11hwReadReg32Ej", wrapHwReadReg32, this->orgHwReadReg32, regDbg},
            {"__ZN13ASIC_INFO__VI18populateDeviceInfoEv", wrapPopulateDeviceInfo, this->orgPopulateDeviceInfo,
                !highsierra},
        };
        PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "gfxcon",
            "Failed to route symbols");

        return true;
    } else if (kextRadeonPolarisCon.loadIndex == index) {
        LRed::callback->setRMMIOIfNecessary();
        auto highsierra = getKernelVersion() == KernelVersion::HighSierra;
        auto regDbg = checkKernelArgument("-lredregdbg");

        RouteRequestPlus requests[] = {
            {"__ZNK22BaffinSharedController11getFamilyIdEv", wrapGetFamilyId, orgGetFamilyId, !highsierra},
            {"__ZN21BaffinRegisterService10hwReadReg8Ej", wrapHwReadReg8, this->orgHwReadReg8, regDbg},
            {"__ZN21BaffinRegisterService11hwReadReg16Ej", wrapHwReadReg16, this->orgHwReadReg16, regDbg},
            {"__ZN21BaffinRegisterService11hwReadReg32Ej", wrapHwReadReg32, this->orgHwReadReg32, regDbg},
            {"__ZN17ASIC_INFO__BAFFIN18populateDeviceInfoEv", wrapPopulateDeviceInfo, this->orgPopulateDeviceInfo,
                !highsierra},
        };
        PANIC_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "gfxcon",
            "Failed to route symbols");

        return true;
    }

    return false;
}

uint8_t GFXCon::wrapHwReadReg8(void *that, uint8_t reg) {
    DBGLOG("gfxcon", "readReg8: reg: %x", reg);
    // turned on by using -lredregdbg
    return FunctionCast(wrapHwReadReg8, callback->orgHwReadReg8)(that, reg);
}

uint16_t GFXCon::wrapHwReadReg16(void *that, uint16_t reg) {
    DBGLOG("gfxcon", "readReg16: reg: %x", reg);
    // turned on by using -lredregdbg
    return FunctionCast(wrapHwReadReg16, callback->orgHwReadReg16)(that, reg);
}

uint32_t GFXCon::wrapHwReadReg32(void *that, uint32_t reg) {
    DBGLOG("gfxcon", "readReg32: reg: %x", reg);
    // turned on by using -lredregdbg
    return FunctionCast(wrapHwReadReg32, callback->orgHwReadReg32)(that, reg);
}

uint16_t GFXCon::wrapGetFamilyId(void) {
    FunctionCast(wrapGetFamilyId, callback->orgGetFamilyId)();
    DBGLOG("gfxcon", "getFamilyId << %x", LRed::callback->isGCN3 ? AMDGPU_FAMILY_CZ : AMDGPU_FAMILY_KV);
    return LRed::callback->isGCN3 ? AMDGPU_FAMILY_CZ : AMDGPU_FAMILY_KV;
}

IOReturn GFXCon::wrapPopulateDeviceInfo(void *that) {
    auto ret = FunctionCast(wrapPopulateDeviceInfo, callback->orgPopulateDeviceInfo)(that);
    getMember<uint32_t>(that, 0x38) = LRed::callback->isGCN3 ? AMDGPU_FAMILY_CZ : AMDGPU_FAMILY_KV;
    getMember<uint32_t>(that, 0x3C) = LRed::callback->deviceId;
    // getMember<uint16_t>(that, 0x40) = LRed::callback->revision;
    getMember<uint32_t>(that, 0x44) =
        // why ChipType instead of ChipVariant? - For mullins we set it as 'Godavari', which is technically just
        // Kalindi+, by the looks of AMDGPU code
        ((LRed::callback->chipType == ChipType::Kalindi)) ?
            static_cast<uint32_t>(LRed::callback->enumeratedRevision) :
            static_cast<uint32_t>(LRed::callback->enumeratedRevision) + LRed::callback->revision;    // rough guess
    return ret;
}
