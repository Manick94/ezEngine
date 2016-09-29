#pragma once

#include <ParticlePlugin/Basics.h>
#include <Foundation/Reflection/Implementation/DynamicRTTI.h>
#include <CoreUtils/DataProcessing/Stream/ProcessingStreamProcessor.h>
#include <ParticlePlugin/Module/ParticleModule.h>

class ezProcessingStream;
class ezParticleSystemInstance;
class ezParticleBehavior;

/// \brief Base class for all particle behaviors
class EZ_PARTICLEPLUGIN_DLL ezParticleBehaviorFactory : public ezReflectedClass
{
  EZ_ADD_DYNAMIC_REFLECTION(ezParticleBehaviorFactory, ezReflectedClass);

public:
  virtual const ezRTTI* GetBehaviorType() const = 0;
  virtual void CopyBehaviorProperties(ezParticleBehavior* pObject) const = 0;

  ezParticleBehavior* CreateBehavior(ezParticleSystemInstance* pOwner) const;

  virtual void Save(ezStreamWriter& stream) const = 0;
  virtual void Load(ezStreamReader& stream) = 0;
};

class EZ_PARTICLEPLUGIN_DLL ezParticleBehavior : public ezParticleModule<ezProcessingStreamProcessor, false>
{
  EZ_ADD_DYNAMIC_REFLECTION(ezParticleBehavior, ezProcessingStreamProcessor);

  friend class ezParticleSystemInstance;

protected:

  virtual void StepParticleSystem(const ezTime& tDiff) { m_TimeDiff = tDiff; }

  ezTime m_TimeDiff;

};

