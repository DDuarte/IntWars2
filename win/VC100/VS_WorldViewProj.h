#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 6.3.9600.16384
//
// Parameters:
//
//   float4x4 WORLD_VIEW_PROJECTION_MATRIX;
//
//
// Registers:
//
//   Name                         Reg   Size
//   ---------------------------- ----- ----
//   WORLD_VIEW_PROJECTION_MATRIX c0       4
//

    vs_3_0
    dcl_position v0
    dcl_texcoord v1
    dcl_position o0
    dcl_texcoord o1.xy
    dp4 o0.x, v0, c0
    dp4 o0.y, v0, c1
    dp4 o0.z, v0, c2
    dp4 o0.w, v0, c3
    mov o1.xy, v1

// approximately 5 instruction slots used
#endif

const BYTE VS_WORLDVIEWPROJ[] =
{
      0,   3, 254, 255, 254, 255, 
     40,   0,  67,  84,  65,  66, 
     28,   0,   0,   0, 103,   0, 
      0,   0,   0,   3, 254, 255, 
      1,   0,   0,   0,  28,   0, 
      0,   0,   0,   1,   0,   0, 
     96,   0,   0,   0,  48,   0, 
      0,   0,   2,   0,   0,   0, 
      4,   0,   2,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
     87,  79,  82,  76,  68,  95, 
     86,  73,  69,  87,  95,  80, 
     82,  79,  74,  69,  67,  84, 
     73,  79,  78,  95,  77,  65, 
     84,  82,  73,  88,   0, 171, 
    171, 171,   3,   0,   3,   0, 
      4,   0,   4,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
    118, 115,  95,  51,  95,  48, 
      0,  77, 105,  99, 114, 111, 
    115, 111, 102, 116,  32,  40, 
     82,  41,  32,  72,  76,  83, 
     76,  32,  83, 104,  97, 100, 
    101, 114,  32,  67, 111, 109, 
    112, 105, 108, 101, 114,  32, 
     54,  46,  51,  46,  57,  54, 
     48,  48,  46,  49,  54,  51, 
     56,  52,   0, 171, 171, 171, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   0,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      0, 128,   1,   0,  15, 144, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   0,   0,  15, 224, 
     31,   0,   0,   2,   5,   0, 
      0, 128,   1,   0,   3, 224, 
      9,   0,   0,   3,   0,   0, 
      1, 224,   0,   0, 228, 144, 
      0,   0, 228, 160,   9,   0, 
      0,   3,   0,   0,   2, 224, 
      0,   0, 228, 144,   1,   0, 
    228, 160,   9,   0,   0,   3, 
      0,   0,   4, 224,   0,   0, 
    228, 144,   2,   0, 228, 160, 
      9,   0,   0,   3,   0,   0, 
      8, 224,   0,   0, 228, 144, 
      3,   0, 228, 160,   1,   0, 
      0,   2,   1,   0,   3, 224, 
      1,   0, 228, 144, 255, 255, 
      0,   0
};
