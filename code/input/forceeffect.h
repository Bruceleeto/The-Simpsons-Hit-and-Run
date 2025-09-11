#ifndef FORCEEFFECT_H
#define FORCEEFFECT_H

#include <radcontroller.hpp>
 

class ForceEffect
{
public:
    ForceEffect() : mOutputPoint(nullptr), mEffectDirty(false) {}
    virtual ~ForceEffect() {}

    void Init(IRadControllerOutputPoint* outputPoint) { mOutputPoint = outputPoint; }
    bool IsInit() const { return (mOutputPoint != nullptr); }

    void Start() {}
    void Stop() {}
    void Update() {}

protected:
    IRadControllerOutputPoint* mOutputPoint;
    bool mEffectDirty;

    virtual void OnInit() = 0;

    // prevent copying
    ForceEffect(const ForceEffect&);
    ForceEffect& operator=(const ForceEffect&);
};

#endif //FORCEEFFECT_H
