#pragma once
#include <memory>
#include <wtypes.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
class MenuWindow;
class ScriptEditorWindow;

class IMGUIManager {
private:
    std::unique_ptr<MenuWindow> menu;
    std::unique_ptr<ScriptEditorWindow> editor;
public:

    explicit IMGUIManager(void);
    ~IMGUIManager(void) noexcept;

    bool init(void);

    void update(void);

    void render(void) const;
};
