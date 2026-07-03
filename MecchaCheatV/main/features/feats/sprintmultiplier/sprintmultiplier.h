#pragma once
#include "Includes.h"

namespace MecchaCheatV::Features::Player
{
    class SprintMultiplier : public FeatureCore
    {
    public:
        SprintMultiplier();
        ~SprintMultiplier() override = default;

        void OnActivate() override {}
        void OnDeactivate() override {}
        void OnRender() override {}
        void OnMenuRender() override;
        void SprintMultiplierHandler();

    private:
        uint64_t GetCurrentFrame();
    };
}