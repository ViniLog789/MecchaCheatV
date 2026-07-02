#pragma once
#include "Includes.h"

namespace MecchaCheatV::Config
{
    void LoadConfig();
    void SaveConfig();
    void LoadConfigFromFile(const std::string& filePath);
    void SaveConfigToFile(const std::string& filePath);
    void MergeConfigWithDefaults(nlohmann::json& existingConfig);

    struct BindConfig
    {
        std::string featureName;
        int key;
    };
}