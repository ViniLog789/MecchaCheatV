#include <chrono>
#include <sstream>
#include <iomanip>
#include "config.h"

namespace MecchaCheatV::Config
{
    void MergeConfigWithDefaults(nlohmann::json& existingConfig)
    {
        nlohmann::json defaultConfig;

        defaultConfig["menu_settings"] = {
            {"toggle_key", Globals::MenuToggleKey}
        };

        if (GET_FEATURE_HANDLER())
        {
            for (const auto& featurePair : GET_FEATURE_HANDLER()->GetFeatures())
            {
                const auto& name = featurePair.first;
                const auto& feature = featurePair.second;

                auto configManager = feature->GetConfigManager();
                nlohmann::json featureJson;

                for (const auto& config : configManager->GetAllConfigs())
                {
                    const auto& configName = config->GetEntryName();
                    const auto& value = config->GetEntryValue();

                    if (std::holds_alternative<bool>(value))
                    {
                        featureJson[configName] = std::get<bool>(value);
                    }
                    else if (std::holds_alternative<int>(value))
                    {
                        featureJson[configName] = std::get<int>(value);
                    }
                    else if (std::holds_alternative<float>(value))
                    {
                        featureJson[configName] = std::get<float>(value);
                    }
                    else if (std::holds_alternative<std::string>(value))
                    {
                        featureJson[configName] = std::get<std::string>(value);
                    }
                    else if (std::holds_alternative<ImColor>(value))
                    {
                        auto color = std::get<ImColor>(value);
                        featureJson[configName] = nlohmann::json{
                            {"r", color.Value.x},
                            {"g", color.Value.y},
                            {"b", color.Value.z},
                            {"a", color.Value.w}
                        };
                    }
                }

                defaultConfig[name] = featureJson;
            }
        }

        std::function<void(nlohmann::json&, const nlohmann::json&)> mergeRecursive =
            [&mergeRecursive](nlohmann::json& target, const nlohmann::json& source)
            {
                if (!source.is_object() || !target.is_object())
                    return;

                for (auto it = source.begin(); it != source.end(); ++it)
                {
                    const auto& key = it.key();
                    const auto& sourceValue = it.value();

                    if (!target.contains(key))
                    {
                        target[key] = sourceValue;
                    }
                    else if (sourceValue.is_object() && target[key].is_object())
                    {
                        mergeRecursive(target[key], sourceValue);
                    }
                }

                auto targetIt = target.begin();
                while (targetIt != target.end())
                {
                    if (!source.contains(targetIt.key()))
                    {
                        targetIt = target.erase(targetIt);
                    }
                    else
                    {
                        ++targetIt;
                    }
                }
            };

        mergeRecursive(existingConfig, defaultConfig);
    }

    void LoadConfigFromFile(const std::string& filePath)
    {
        using Json = nlohmann::json;

        if (!std::filesystem::exists(filePath))
        {
            SaveConfigToFile(filePath);
            return;
        }

        try
        {
            std::ifstream f(filePath);

            if (f.peek() == std::ifstream::traits_type::eof())
            {
                f.close();
                SaveConfigToFile(filePath);
                return;
            }

            Json data = Json::parse(f);
            f.close();

            MergeConfigWithDefaults(data);

            if (data.contains("menu_settings") && data["menu_settings"].contains("toggle_key"))
            {
                Globals::MenuToggleKey = data["menu_settings"]["toggle_key"].get<int>();
            }

            if (GET_FEATURE_HANDLER())
            {
                for (const auto& featurePair : GET_FEATURE_HANDLER()->GetFeatures())
                {
                    const auto& name = featurePair.first;
                    const auto& feature = featurePair.second;

                    auto configManager = feature->GetConfigManager();

                    if (data.contains(name))
                    {
                        for (const auto& config : configManager->GetAllConfigs())
                        {
                            const auto& configName = config->GetEntryName();

                            if (data[name].contains(configName))
                            {
                                const auto& configValue = data[name][configName];
                                auto& value = config->GetEntryValue();

                                if (std::holds_alternative<bool>(value) && configValue.is_boolean())
                                {
                                    config->SetEntryValue(configValue.get<bool>());
                                }
                                else if (std::holds_alternative<int>(value) && configValue.is_number_integer())
                                {
                                    config->SetEntryValue(configValue.get<int>());
                                }
                                else if (std::holds_alternative<float>(value) && configValue.is_number_float())
                                {
                                    config->SetEntryValue(configValue.get<float>());
                                }
                                else if (std::holds_alternative<std::string>(value) && configValue.is_string())
                                {
                                    config->SetEntryValue(configValue.get<std::string>());
                                }
                                else if (std::holds_alternative<ImColor>(value) && configValue.is_object())
                                {
                                    ImColor color(
                                        configValue["r"].get<float>(),
                                        configValue["g"].get<float>(),
                                        configValue["b"].get<float>(),
                                        configValue["a"].get<float>()
                                    );
                                    config->SetEntryValue(color);
                                }
                            }
                        }
                    }
                }
            }

            std::ofstream outFile(filePath);
            outFile << data.dump(4);
            outFile.close();
        }
        catch (const nlohmann::json::parse_error& e)
        {
            SaveConfigToFile(filePath);
        }
        catch (const std::exception& e) {}
    }

    void SaveConfigToFile(const std::string& filePath)
    {
        using Json = nlohmann::json;
        Json data;

        Json menuSettings;
        menuSettings["toggle_key"] = Globals::MenuToggleKey;
        data["menu_settings"] = menuSettings;

        if (GET_FEATURE_HANDLER())
        {
            for (const auto& featurePair : GET_FEATURE_HANDLER()->GetFeatures())
            {
                const auto& name = featurePair.first;
                const auto& feature = featurePair.second;

                auto configManager = feature->GetConfigManager();
                Json featureJson;

                for (const auto& config : configManager->GetAllConfigs())
                {
                    const auto& configName = config->GetEntryName();
                    const auto& value = config->GetEntryValue();

                    if (std::holds_alternative<bool>(value))
                    {
                        featureJson[configName] = std::get<bool>(value);
                    }
                    else if (std::holds_alternative<int>(value))
                    {
                        featureJson[configName] = std::get<int>(value);
                    }
                    else if (std::holds_alternative<float>(value))
                    {
                        featureJson[configName] = std::get<float>(value);
                    }
                    else if (std::holds_alternative<std::string>(value))
                    {
                        featureJson[configName] = std::get<std::string>(value);
                    }
                    else if (std::holds_alternative<ImColor>(value))
                    {
                        auto color = std::get<ImColor>(value);
                        featureJson[configName] = Json{
                            {"r", color.Value.x},
                            {"g", color.Value.y},
                            {"b", color.Value.z},
                            {"a", color.Value.w}
                        };
                    }
                }

                data[name] = featureJson;
            }
        }

        std::ofstream f(filePath);
        f << data.dump(4);
        f.close();
    }

    void LoadConfig()
    {
        const auto configPath = Utils::GetCheatDirectory() + "\\config.json";
        LoadConfigFromFile(configPath);
    }

    void SaveConfig()
    {
        const auto configPath = Utils::GetCheatDirectory() + "\\config.json";
        SaveConfigToFile(configPath);
    }
}