

#include "3Dworld/3Dworld.h"
#include "3Dworld/3Dworld.skia.h"
#include "3Dworld/gainput/gainput.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkGradientShader.h"

using namespace sk_app;
using namespace world;

currentworld curwor;
gainput::InputManager manager;
const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();

Application* Application::Create(int argc, char** argv, void* platformData) {
    return new _3Dworld(argc, argv, platformData);
}

_3Dworld::_3Dworld(int argc, char** argv, void* platformData)
        : fBackendType(Window::kNativeGL_BackendType), fRotationAngle(0) {
    SkGraphics::Init();

    fWindow = Window::CreateNativeWindow(platformData);
    fWindow->setRequestedDisplayParams(DisplayParams());

    // register callbacks
    fWindow->pushLayer(this);

    fWindow->attach(fBackendType);
    manager.SetDisplaySize(fWindow->height(), fWindow->width());
    mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
}

_3Dworld::~_3Dworld() {
    fWindow->detach();
    delete fWindow;
}

void _3Dworld::updateTitle() {
    if (!fWindow || fWindow->sampleCount() <= 1) {
        return;
    }

    SkString title("Hello World ");
    title.append(Window::kRaster_BackendType == fBackendType ? "Raster" : "OpenGL");
    fWindow->setTitle(title.c_str());
}

void _3Dworld::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
}

void _3Dworld::onPaint(SkSurface* surface) {
    SkCanvas* canvas = surface->getCanvas();
    // Clear background
    canvas->clear(SK_ColorBLACK);

    if (gainput::InputDeviceMouse::GetBool(gainput::MouseButton3)) {
        curwor.naer_clipping_plane += 0.2;
    }
    if (gainput::InputDeviceMouse::GetBool(gainput::MouseButton4)) {
        curwor.naer_clipping_plane -= 0.2;
    }

    curwor.renderscreen(canvas);

    canvas->restore();
}

void _3Dworld::onIdle() {
    manager.Update();
    // Just re-paint continously
    fWindow->inval();
}

bool _3Dworld::onChar(SkUnichar c, skui::ModifierKey modifiers) {
    if (' ' == c) {
        fBackendType = Window::kRaster_BackendType == fBackendType ? Window::kNativeGL_BackendType
                                                                   : Window::kRaster_BackendType;
        fWindow->detach();
        fWindow->attach(fBackendType);
    }
    return true;
}
