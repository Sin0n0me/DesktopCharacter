#pragma once
#include "../vertex/PMDVertexData.h"
#include "Morph.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class IMorphAccessor {
public:
    virtual ~IMorphAccessor(void) noexcept = default;

    virtual std::shared_ptr<std::vector<PMDVertexData>> get_mutable_vertices(void) const noexcept = 0;

    virtual std::shared_ptr<const std::vector<PMDMorphData>> get_morphs(void) const noexcept = 0;

    virtual std::optional<uint32_t> get_morph_index(const std::string& name) const noexcept = 0;
};
