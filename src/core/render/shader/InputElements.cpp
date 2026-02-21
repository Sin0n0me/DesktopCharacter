#include "InputElements.h"

void InputElements::add(
    const D3D11_INPUT_ELEMENT_DESC& desc
) {
    this->name.push_back(desc.SemanticName);
    this->desc.push_back(desc);
    for(size_t i = 0; i < this->desc.size(); ++i) {
        this->desc[i] = this->desc[i];
        this->desc[i].SemanticName = this->name[i].c_str();
    }
}

const std::vector<D3D11_INPUT_ELEMENT_DESC>& InputElements::get_elements(void) const {
    return this->desc;
}