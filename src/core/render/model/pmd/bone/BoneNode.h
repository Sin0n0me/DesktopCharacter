#pragma once
#include <DirectXMath.h>
#include <vector>

class BoneNode {
protected:
    BoneNode* parent;
    std::vector<BoneNode*> children;

public: // TODO: public to protected
    float angle;
    DirectX::XMVECTOR rotate;
    DirectX::XMMATRIX local;
    DirectX::XMMATRIX global;

public:

    explicit BoneNode(void);

    void update_local(void);

    void update_global(void);

    void update_(void);
};
