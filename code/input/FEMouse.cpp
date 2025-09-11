#ifndef FEMOUSE_H
#define FEMOUSE_H

#include <presentation/gui/guiinputdefs.h> 

class tDrawable; 
class MouseCursor
{
public:
    void Set(tDrawable*) {}
    void SetPos(float, float) {}
    bool IsVisible() const { return false; }
    void SetVisible(bool) {}
    float XPos() const { return 0.0f; }
    float YPos() const { return 0.0f; }
    void Render() {}
};

class FEMouse
{
public:
    FEMouse() {}
    ~FEMouse() {}

    void InitMouseCursor(tDrawable*) {}
    eFEMouseHorzDir OnSliderHorizontalClickDrag() const { return NO_HORZ_MOVEMENT; }
    bool DidWeMove(int, int) const { return false; }
    void Move(int, int, long, long) {}
    void Update() {}
    void ButtonDown(eFEMouseButton) {}
    void ButtonUp(eFEMouseButton) {}
    void SetInGameMode(bool) {}
    void SetInGameOverride(bool) {}

private:
    void SetupInGameMode() {}
};

#endif // FEMOUSE_H
