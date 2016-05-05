#include "Main.h"


ezResult ezTexConv::LoadSingleInputFile(const char* szFile)
{
  ezImage& source = m_InputImages.ExpandAndGetRef();
  if (source.LoadFrom(szFile).Failed())
  {
    ezLog::Error("Failed to load file '%s'", szFile);
    return EZ_FAILURE;
  }

  return EZ_SUCCESS;
}

ezResult ezTexConv::LoadInputs()
{
  EZ_LOG_BLOCK("Load Inputs");

  m_InputImages.Reserve(m_InputFileNames.GetCount());

  for (const auto& in : m_InputFileNames)
  {
    if (LoadSingleInputFile(in).Failed())
      return EZ_FAILURE;
  }

  for (ezUInt32 i = 1; i < m_InputImages.GetCount(); ++i)
  {
    if (m_InputImages[i].GetWidth() != m_InputImages[0].GetWidth() ||
        m_InputImages[i].GetHeight() != m_InputImages[0].GetHeight())
    {
      ezLog::Error("Input image %u has a different resolution than image 0. This is currently not supported.", i);
      return EZ_FAILURE;
    }
  }

  return EZ_SUCCESS;
}

ezResult ezTexConv::ConvertInputsToRGBA()
{
  for (ezUInt32 i = 0; i < m_InputImages.GetCount(); ++i)
  {
    if (ezImageConversion::Convert(m_InputImages[i], m_InputImages[i], ezImageFormat::R8G8B8A8_UNORM).Failed())
    {
      ezLog::Error("Failed to convert input %i from format %s to R8G8B8A8_UNORM. Format is not supported.", i, ezImageFormat::GetName(m_InputImages[i].GetImageFormat()));
      return EZ_FAILURE;
    }
  }

  return EZ_SUCCESS;
}

ezImage* ezTexConv::CreateCombinedFile(const ChannelMapping* dataSources)
{
  /// \todo Handle different input sizes

  ezImage* pImg = EZ_DEFAULT_NEW(ezImage);
  m_CleanupImages.PushBack(pImg);

  /// \todo Return loaded image pointer, if no combination is necessary

  const ezUInt32 uiWidth = m_InputImages[0].GetWidth();
  const ezUInt32 uiHeight = m_InputImages[0].GetHeight();

  pImg->SetWidth(uiWidth);
  pImg->SetHeight(uiHeight);
  pImg->SetDepth(1);
  pImg->SetNumArrayIndices(1);
  pImg->SetNumFaces(1);
  pImg->SetNumMipLevels(1);
  pImg->SetImageFormat(ezImageFormat::R8G8B8A8_UNORM);
  pImg->AllocateImageData();

  // later block compression may pre-multiply rgb by alpha, if we have never set alpha to anything (3 channel case), that will result in black
  ezUInt32 uiDefaultResult = 0;
  if (m_uiOutputChannels < 4)
    uiDefaultResult = 0xFF000000;

  // not the most efficient loops...
  for (ezUInt32 h = 0; h < uiHeight; ++h)
  {
    for (ezUInt32 w = 0; w < uiWidth; ++w)
    {
      ezUInt32 uiResultRGBA = uiDefaultResult;

      for (ezUInt32 i = 0; i < m_uiOutputChannels; ++i)
      {
        const auto& ds = dataSources[i];

        float channelValue = 0.0f;

        if (ds.m_iInput == -1)
        {
          // handles 'black' and 'white' values

          if (ds.m_uiChannelMask == Channel::All)
            channelValue = 1.0f;
        }
        else
        {
          const ezImage& src = m_InputImages[ds.m_iInput];
          const ezUInt32 rgba = *src.GetPixelPointer<ezUInt32>(0, 0, 0, w, h, 0);

          channelValue = GetChannelValue(ds, rgba);
        }

        // we build all images in linear space and set the SRGB format at the end (or not)

        const ezUInt32 uiFinalUB = ezMath::ColorFloatToByte(channelValue);

        uiResultRGBA |= uiFinalUB << (i * 8);
      }

      *pImg->GetPixelPointer<ezUInt32>(0, 0, 0, w, h, 0) = uiResultRGBA;
    }
  }

  return pImg;
}