#include <PCH.h>
#include <EnginePluginParticle/ParticleAsset/ParticleView.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <RendererCore/RenderLoop/RenderLoop.h>
#include <RendererCore/Pipeline/View.h>
#include <GameFoundation/GameApplication/GameApplication.h>
#include <EditorFramework/EngineProcess/EngineProcessDocumentContext.h>
#include <EditorFramework/EngineProcess/EngineProcessMessages.h>

ezParticleViewContext::ezParticleViewContext(ezParticleContext* pParticleContext) : ezEngineProcessViewContext(pParticleContext)
{
  m_pParticleContext = pParticleContext;
  m_pView = nullptr;
}

ezParticleViewContext::~ezParticleViewContext()
{
  ezRenderLoop::DeleteView(m_pView);

  if (GetEditorWindow().m_hWnd != 0)
  {
    static_cast<ezGameApplication*>(ezApplication::GetApplicationInstance())->RemoveWindow(&GetEditorWindow());
  }
}

void ezParticleViewContext::PositionThumbnailCamera()
{
  m_Camera.SetCameraMode(ezCameraMode::PerspectiveFixedFovX, 45.0f, 0.1f, 1000.0f);
  m_Camera.LookAt(ezVec3(-1.8f, 1.8f, 1.0f), ezVec3::ZeroVector(), ezVec3(0.0f, 0.0f, 1.0f));
}

ezView* ezParticleViewContext::CreateView()
{
  ezView* pView = ezRenderLoop::CreateView("Particle Editor - View");

  pView->SetRenderPipelineResource(CreateDefaultRenderPipeline());

  ezEngineProcessDocumentContext* pDocumentContext = GetDocumentContext();
  pView->SetWorld(pDocumentContext->GetWorld());
  pView->SetLogicCamera(&m_Camera);
  return pView;
}