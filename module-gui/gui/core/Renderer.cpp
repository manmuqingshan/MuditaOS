// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "Renderer.hpp"
#include "Context.hpp"

#include "renderers/PixelRenderer.hpp"

namespace gui
{
    void Renderer::changeColorScheme(const std::unique_ptr<ColorScheme> &scheme) const
    {
        renderer::PixelRenderer::updateColorScheme(scheme);
    }

    void Renderer::render(Context *ctx, const std::list<std::unique_ptr<DrawCommand>> &commands) const
    {
        if (ctx == nullptr) {
            return;
        }

        for (auto &cmd : commands) {
            if (cmd == nullptr) {
                continue;
            }

            cmd->draw(ctx);
        }
    }
} /* namespace gui */
