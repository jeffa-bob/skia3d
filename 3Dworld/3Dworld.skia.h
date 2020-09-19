

#ifndef HelloWorld_DEFINED
#define HelloWorld_DEFINED

#include "tools/sk_app/Application.h"
#include "tools/sk_app/Window.h"

class SkCanvas;

class _3Dworld : public sk_app::Application, sk_app::Window::Layer {
public:
    _3Dworld(int argc, char** argv, void* platformData);
    ~_3Dworld() override;

    void onIdle() override;

    void onBackendCreated() override;
    void onPaint(SkSurface*) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;

private:
    void updateTitle();

    sk_app::Window* fWindow;
    sk_app::Window::BackendType fBackendType;

    SkScalar fRotationAngle;
};

#endif
