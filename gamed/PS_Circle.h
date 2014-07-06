#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 6.3.9600.16384
//
// Parameters:
//
//   float4 InnerColor;
//   float4 OuterColor;
//   float4 Radius;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Radius       c0       1
//   InnerColor   c1       1
//   OuterColor   c2       1
//

    ps_2_0
    def c3, -0.5, 0, 0, 0
    dcl t0.xy
    add r0.xy, t0, c3.x
    dp2add r0.x, r0, r0, c3.y
    rsq r0.x, r0.x
    rcp r0.x, r0.x
    mad r1, r0.x, -c0.z, c0.y
    mad r0, r0.x, c0.z, -c0.x
    texkill r1
    texkill r0
    rcp r0.x, c0.w
    mul_sat r0.y, r0.x, r1.w
    mul_sat r0.x, r0.x, r0.w
    add r0.z, -c0.x, c0.y
    rcp r0.z, r0.z
    mul r0.z, r0.z, r0.w
    mov r1, c1
    add r1, -r1, c2
    mad r1, r0.z, r1, c1
    mul r0.y, r0.y, r1.w
    mul r1.w, r0.x, r0.y
    mov oC0, r1

// approximately 21 instruction slots used
#endif

const BYTE PS_CIRCLE[] =
{
      0,   2, 255, 255, 254, 255, 
     49,   0,  67,  84,  65,  66, 
     28,   0,   0,   0, 141,   0, 
      0,   0,   0,   2, 255, 255, 
      3,   0,   0,   0,  28,   0, 
      0,   0,   0,   1,   0,   0, 
    134,   0,   0,   0,  88,   0, 
      0,   0,   2,   0,   1,   0, 
      1,   0,   6,   0, 100,   0, 
      0,   0,   0,   0,   0,   0, 
    116,   0,   0,   0,   2,   0, 
      2,   0,   1,   0,  10,   0, 
    100,   0,   0,   0,   0,   0, 
      0,   0, 127,   0,   0,   0, 
      2,   0,   0,   0,   1,   0, 
      2,   0, 100,   0,   0,   0, 
      0,   0,   0,   0,  73, 110, 
    110, 101, 114,  67, 111, 108, 
    111, 114,   0, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  79, 117, 116, 101, 
    114,  67, 111, 108, 111, 114, 
      0,  82,  97, 100, 105, 117, 
    115,   0, 112, 115,  95,  50, 
     95,  48,   0,  77, 105,  99, 
    114, 111, 115, 111, 102, 116, 
     32,  40,  82,  41,  32,  72, 
     76,  83,  76,  32,  83, 104, 
     97, 100, 101, 114,  32,  67, 
    111, 109, 112, 105, 108, 101, 
    114,  32,  54,  46,  51,  46, 
     57,  54,  48,  48,  46,  49, 
     54,  51,  56,  52,   0, 171, 
     81,   0,   0,   5,   3,   0, 
     15, 160,   0,   0,   0, 191, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   0,   0,   3, 176, 
      2,   0,   0,   3,   0,   0, 
      3, 128,   0,   0, 228, 176, 
      3,   0,   0, 160,  90,   0, 
      0,   4,   0,   0,   1, 128, 
      0,   0, 228, 128,   0,   0, 
    228, 128,   3,   0,  85, 160, 
      7,   0,   0,   2,   0,   0, 
      1, 128,   0,   0,   0, 128, 
      6,   0,   0,   2,   0,   0, 
      1, 128,   0,   0,   0, 128, 
      4,   0,   0,   4,   1,   0, 
     15, 128,   0,   0,   0, 128, 
      0,   0, 170, 161,   0,   0, 
     85, 160,   4,   0,   0,   4, 
      0,   0,  15, 128,   0,   0, 
      0, 128,   0,   0, 170, 160, 
      0,   0,   0, 161,  65,   0, 
      0,   1,   1,   0,  15, 128, 
     65,   0,   0,   1,   0,   0, 
     15, 128,   6,   0,   0,   2, 
      0,   0,   1, 128,   0,   0, 
    255, 160,   5,   0,   0,   3, 
      0,   0,  18, 128,   0,   0, 
      0, 128,   1,   0, 255, 128, 
      5,   0,   0,   3,   0,   0, 
     17, 128,   0,   0,   0, 128, 
      0,   0, 255, 128,   2,   0, 
      0,   3,   0,   0,   4, 128, 
      0,   0,   0, 161,   0,   0, 
     85, 160,   6,   0,   0,   2, 
      0,   0,   4, 128,   0,   0, 
    170, 128,   5,   0,   0,   3, 
      0,   0,   4, 128,   0,   0, 
    170, 128,   0,   0, 255, 128, 
      1,   0,   0,   2,   1,   0, 
     15, 128,   1,   0, 228, 160, 
      2,   0,   0,   3,   1,   0, 
     15, 128,   1,   0, 228, 129, 
      2,   0, 228, 160,   4,   0, 
      0,   4,   1,   0,  15, 128, 
      0,   0, 170, 128,   1,   0, 
    228, 128,   1,   0, 228, 160, 
      5,   0,   0,   3,   0,   0, 
      2, 128,   0,   0,  85, 128, 
      1,   0, 255, 128,   5,   0, 
      0,   3,   1,   0,   8, 128, 
      0,   0,   0, 128,   0,   0, 
     85, 128,   1,   0,   0,   2, 
      0,   8,  15, 128,   1,   0, 
    228, 128, 255, 255,   0,   0
};