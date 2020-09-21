

#include "3Dworld/3Dworld.skia.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkGradientShader.h"
#include "3Dworld/3Dworld.h"

using namespace sk_app;
using namespace world;

currentworld curwor;

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

    curwor.renderscreen(canvas);

    canvas->restore();
}

void _3Dworld::onIdle() {
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
    switch (c)
    {
    case 'i':
        curwor.cam.pos.z += 0.2f;
        break;
    case 'j':
        curwor.cam.pos.x += 0.2f;
        break;
    case 'k':
        curwor.cam.pos.z += 0.2f;
        break;
    case 'l':
        curwor.cam.pos.x -= 0.2f;
        break;
    case 'u':
        curwor.cam.pos.y += 0.2f;
        break;
    case 'o':
        curwor.cam.pos.y -= 0.2f;
        break;
    case 'p':
        curwor.naer_clipping_plane += 0.2f;
        break;
    case ';':
        curwor.naer_clipping_plane -= 0.2f;
        break;
    default:
        break;
    }
    return true;
}
