Node %PointSampler
{
  string %Category { "Texturing/Samplers" }
  unsigned_int8 %Color { 0, 96, 96 }

  string %CodePixelSamplers { "PointSampler;" }

  OutputPin %Sampler
  {
    string %Type { "sampler" }
    unsigned_int8 %Color { 0, 96, 96 }
    string %Inline { "PointSampler" }
  }
}

Node %LinearSampler
{
  string %Category { "Texturing/Samplers" }
  unsigned_int8 %Color { 0, 96, 96 }

  string %CodePixelSamplers { "SamplerState LinearSampler;" }

  OutputPin %Sampler
  {
    string %Type { "sampler" }
    unsigned_int8 %Color { 0, 96, 96 }
    string %Inline { "LinearSampler" }
  }
}

Node %PointClampSampler
{
  string %Category { "Texturing/Samplers" }
  unsigned_int8 %Color { 0, 96, 96 }

  string %CodePixelSamplers { "SamplerState PointClampSampler;" }

  OutputPin %Sampler
  {
    string %Type { "sampler" }
    unsigned_int8 %Color { 0, 96, 96 }
    string %Inline { "PointClampSampler" }
  }
}

Node %LinearClampSampler
{
  string %Category { "Texturing/Samplers" }
  unsigned_int8 %Color { 0, 96, 96 }

  string %CodePixelSamplers { "SamplerState LinearClampSampler;" }

  OutputPin %Sampler
  {
    string %Type { "sampler" }
    unsigned_int8 %Color { 0, 96, 96 }
    string %Inline { "LinearClampSampler" }
  }
}
