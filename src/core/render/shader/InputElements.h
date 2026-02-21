#pragma once
#include <d3d11.h>
#include <string>
#include <vector>

// ダングリングポインタになってしまうので保持用に
class InputElements {
private:
    std::vector<std::string> name;
    std::vector<D3D11_INPUT_ELEMENT_DESC> desc;

public:

    void add(const D3D11_INPUT_ELEMENT_DESC& desc);

    const std::vector<D3D11_INPUT_ELEMENT_DESC>& get_elements(void) const;
};
