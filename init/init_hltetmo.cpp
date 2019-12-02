/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.
   Copyright (c) 2017-2018, The LineageOS Project. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <android-base/logging.h>
#include <android-base/properties.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using android::init::property_set;

// copied from build/tools/releasetools/ota_from_target_files.py
// but with "." at the end and empty entry
std::vector<std::string> ro_product_props_default_source_order = {
    ".",
    "product.",
    "product_services.",
    "odm.",
    "vendor.",
    "system.",
};

void property_override(char const prop[], char const value[], bool add = true)
{
    auto pi = (prop_info *) __system_property_find(prop);

    if (pi != nullptr) {
        __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

void cdma_properties(char const *operator_alpha,
        char const *operator_numeric,
        char const *default_network,
        char const *cdma_sub)
{
    /* Dynamic CDMA Properties */
    property_set("ro.cdma.home.operator.alpha", operator_alpha);
    property_set("ro.cdma.home.operator.numeric", operator_numeric);
    property_set("ro.telephony.default_network", default_network);
    property_set("ro.telephony.default_cdma_sub", cdma_sub);

    /* Static CDMA Properties */
    property_set("ril.subscription.types", "NV,RUIM");
    property_set("telephony.lteOnCdmaDevice", "1");
}

void gsm_properties()
{
    property_set("ro.telephony.default_network", "9");
    property_set("telephony.lteOnGsmDevice", "1");
}

void vendor_load_properties() {

    std::string bootloader = GetProperty("ro.bootloader", "");
    
    const auto set_ro_product_prop = [](const std::string &source,
            const std::string &prop, const std::string &value) {
        auto prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str(), false);
    }; 

    if (bootloader.find("N900R4") == 0) {
        /* hlteusc */
          for (const auto &source : ro_product_props_default_source_order) {
            set_ro_product_prop(source, "fingerprint", "samsung/hlteusc/hlteusc:5.0/LRX21V/N900R4TYUDPE2:user/release-keys");
            set_ro_product_prop(source, "device", "hlteusc");
            set_ro_product_prop(source, "model", "SM-N900R4");
        }     
        property_override("ro.build.description", "hlteusc-user 5.0 LRX21V N900R4TYUDPE2 release-keys");
        cdma_properties("U.S. Cellular", "311220", "10", "0");
    } else if (bootloader.find("N900T") == 0) {
        /* hltetmo */
       for (const auto &source : ro_product_props_default_source_order) {
            set_ro_product_prop(source, "fingerprint", "samsung/hltetmo/hltetmo:5.0/LRX21V/N900TUVUFQD2:user/release-keys");
            set_ro_product_prop(source, "device", "hltetmo");
            set_ro_product_prop(source, "model", "SM-N900T");
        }
        property_override("ro.build.description", "hltetmo-user 5.0 LRX21V N900TUVUFQD2 release-keys");
        gsm_properties();
    } else if (bootloader.find("N900V") == 0) {
        /* hltevzw */
       for (const auto &source : ro_product_props_default_source_order) {
            set_ro_product_prop(source, "fingerprint", "Verizon/hltevzw/hltevzw:5.0/LRX21V/N900VVRSEPL1:user/release-keys");
            set_ro_product_prop(source, "device", "hltevzw");
            set_ro_product_prop(source, "model", "SM-N900V");
        }
        property_override("ro.build.description", "hltevzw-user 5.0 LRX21V N900VVRSEPL1 release-keys");
        cdma_properties("Verizon", "311480", "10", "1");
    } else if (bootloader.find("N900W8") == 0) {
        /* hltecan */
       for (const auto &source : ro_product_props_default_source_order) {
            set_ro_product_prop(source, "fingerprint", "samsung/hltevl/hltecan:5.0/LRX21V/N900W8VLU2DPG1:user/release-keys");
            set_ro_product_prop(source, "device", "hltecan");
            set_ro_product_prop(source, "model", "SM-N900W8");
        }
        property_override("ro.build.description", "hltevl-user 5.0 LRX21V N900W8VLU2DPG1 release-keys");
        gsm_properties();
    } else {
        gsm_properties();
    }

    std::string device = GetProperty("ro.product.device", "");
    LOG(ERROR) << "Found bootloader id " << bootloader <<  " setting build properties for "
        << device <<  " device" << std::endl;
}
