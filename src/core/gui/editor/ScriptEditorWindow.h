#pragma once

#include <vector>

struct ImFont;

class ScriptEditorWindow {
private:

    std::vector<char> text_buffer;
    ImFont* font;

    void on_clicked_compile(void);

public:

    explicit ScriptEditorWindow(void);

    bool init(void);

    void update(void);
};
