#include <PCH.h>
#include <EditorPluginAssets/TextureAsset/TextureAsset.h>
#include <EditorPluginAssets/TextureAsset/TextureAssetObjects.h>
#include <EditorPluginAssets/TextureAsset/TextureAssetManager.h>
#include <EditorPluginAssets/TextureAsset/TextureAssetObjectsManager.h>
#include <ToolsFoundation/Reflection/ReflectedTypeManager.h>
#include <EditorFramework/Assets/AssetCurator.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <CoreUtils/Image/Formats/DdsFileFormat.h>
#include <CoreUtils/Image/ImageConversion.h>

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezTextureAssetDocument, ezAssetDocument, 1, ezRTTINoAllocator);
EZ_END_DYNAMIC_REFLECTED_TYPE();

ezTextureAssetDocument::ezTextureAssetDocument(const char* szDocumentPath) : ezAssetDocument(szDocumentPath, EZ_DEFAULT_NEW(ezTextureAssetObjectManager))
{
}

ezTextureAssetDocument::~ezTextureAssetDocument()
{
}

const ezTextureAssetProperties* ezTextureAssetDocument::GetProperties() const
{
  ezTextureAssetObject* pObject = (ezTextureAssetObject*) GetObjectTree()->GetRootObject()->GetChildren()[0];
  return &pObject->m_MemberProperties;
}

void ezTextureAssetDocument::Initialize()
{
  ezAssetDocument::Initialize();

  EnsureSettingsObjectExist();
}

void ezTextureAssetDocument::EnsureSettingsObjectExist()
{
  auto pRoot = GetObjectTree()->GetRootObject();
  if (pRoot->GetChildren().IsEmpty())
  {
    ezTextureAssetObject* pObject = static_cast<ezTextureAssetObject*>(GetObjectManager()->CreateObject(ezReflectedTypeManager::GetTypeHandleByName(ezGetStaticRTTI<ezTextureAssetProperties>()->GetTypeName())));

    GetObjectTree()->AddObject(pObject, pRoot);
  }

}

ezStatus ezTextureAssetDocument::InternalLoadDocument()
{
  GetObjectTree()->DestroyAllObjects(GetObjectManager());

  ezStatus ret = ezAssetDocument::InternalLoadDocument();

  EnsureSettingsObjectExist();

  return ret;
}

void ezTextureAssetDocument::UpdateAssetDocumentInfo(ezAssetDocumentInfo* pInfo)
{
  pInfo->m_FileDependencies.Clear();

  const ezTextureAssetProperties* pProp = GetProperties();

  ezStringBuilder sTemp = pProp->GetInputFile();
  sTemp.MakeCleanPath();

  pInfo->m_FileDependencies.PushBack(sTemp);

  pInfo->m_uiSettingsHash = GetDocumentHash();
}

ezStatus ezTextureAssetDocument::InternalTransformAsset(ezStreamWriterBase& stream, const char* szPlatform)
{
  EZ_ASSERT_DEV(ezStringUtils::IsEqual(szPlatform, "PC"), "Platform '%s' is not supported", szPlatform);

  const ezImage* pImage = &GetProperties()->GetImage();
  ezImage ConvertedImage;

  stream << GetProperties()->IsSRGB();

  ezImageFormat::Enum TargetFormat = pImage->GetImageFormat();

  if (pImage->GetImageFormat() == ezImageFormat::B8G8R8_UNORM)
  {
    /// \todo A conversion to B8G8R8X8_UNORM currently fails
    TargetFormat = ezImageFormat::B8G8R8A8_UNORM;
  }

  if (pImage->GetImageFormat() == ezImageFormat::A8_UNORM)
  {
    // convert alpha channel only format to red channel only
    TargetFormat = ezImageFormat::R8_UNORM;
  }

  if (TargetFormat != pImage->GetImageFormat())
  {
    if (ezImageConversionBase::Convert(*pImage, ConvertedImage, TargetFormat).Failed())
      return ezStatus("Conversion to from source format '%s' to target format '%s' failed", ezImageFormat::GetName(pImage->GetImageFormat()), ezImageFormat::GetName(TargetFormat));

    pImage = &ConvertedImage;
  }

  ezDdsFileFormat writer;
  if (writer.WriteImage(stream, *pImage, ezGlobalLog::GetInstance()).Failed())
  {
    return ezStatus("Writing the image data as DDS failed");
  }

  SaveThumbnail(*pImage);

  return ezStatus(EZ_SUCCESS);
}

