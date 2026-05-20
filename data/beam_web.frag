#version 100

precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform float time;
uniform float beamLength;

#define BEAM_SCROLL_SPEED 18.0
#define FLICKER_FAST_FREQ 34.0
#define FLICKER_SLOW_FREQ 11.0
#define FLICKER_STEPS 8.0
#define CORE_WIDTH 0.12
#define BEAM_WIDTH 0.30
#define GLOW_WIDTH 0.82
#define EDGE_FADE_LENGTH 0.055

#define MUZZLE_POSITION_X 0.055
#define MUZZLE_LENGTH 0.43
#define MUZZLE_WIDTH 0.95
#define MUZZLE_CORE_WIDTH 0.28
#define MUZZLE_GLOW_INTENSITY 1.8
#define MUZZLE_CORE_INTENSITY 2.2
#define MUZZLE_FLICKER_AMOUNT 0.45

#define SHEATH_START_WIDTH 0.95
#define SHEATH_END_WIDTH 0.34
#define SHEATH_FADE_AMOUNT 0.72
#define SHEATH_INTENSITY 0.9
#define SHEATH_ALPHA 0.42

void main()
{
    vec2 uv = fragTexCoord;
    float dist = abs(uv.y - 0.5) * 2.0;
    float alongBeam = uv.x * max(beamLength, 1.0) * 0.035 + time * BEAM_SCROLL_SPEED;

    float flicker = sin(alongBeam * FLICKER_FAST_FREQ) * 0.5 + sin(alongBeam * FLICKER_SLOW_FREQ) * 0.5;
    flicker = floor(flicker * FLICKER_STEPS) / FLICKER_STEPS;
    flicker = 0.85 + 0.45 * flicker;

    float core = smoothstep(CORE_WIDTH, 0.0, dist) * (1.2 + flicker);
    float beam = smoothstep(BEAM_WIDTH, 0.0, dist) * flicker;
    float glow = smoothstep(GLOW_WIDTH, 0.0, dist) * 0.55;

    float startFade = smoothstep(0.0, EDGE_FADE_LENGTH, uv.x);
    float endFade = smoothstep(0.0, EDGE_FADE_LENGTH, 1.0 - uv.x);
    float edgeFade = startFade * endFade;
    core *= edgeFade;
    beam *= edgeFade;
    glow *= edgeFade;

    vec2 muzzleUv = vec2((uv.x - MUZZLE_POSITION_X) * 3.2, (uv.y - 0.5) * 2.0);
    float muzzleDist = length(muzzleUv);
    float muzzleMask = smoothstep(0.0, EDGE_FADE_LENGTH, uv.x) * smoothstep(0.0, EDGE_FADE_LENGTH * 1.4, MUZZLE_LENGTH - uv.x);
    float muzzleGlow = smoothstep(MUZZLE_WIDTH, 0.0, muzzleDist) * muzzleMask * (0.7 + MUZZLE_FLICKER_AMOUNT * flicker);
    float muzzleCore = smoothstep(MUZZLE_CORE_WIDTH, 0.0, muzzleDist) * muzzleMask * (1.0 + flicker);

    float sheathWidth = mix(SHEATH_START_WIDTH, SHEATH_END_WIDTH, uv.x);
    float sheathPower = 1.0 - uv.x * SHEATH_FADE_AMOUNT;
    float sheathNoise = 0.82 + 0.28 * sin(alongBeam * 1.7 + dist * 9.0);
    float sheath = smoothstep(sheathWidth, 0.0, dist) * edgeFade * sheathPower * sheathNoise;

    float scan = 0.94 + 0.06 * sin((uv.y + time * 0.8) * 95.0);
    float noise = fract(sin(dot(floor(vec2(uv.x * 180.0 + time * 32.0, uv.y * 32.0)), vec2(12.9898, 78.233))) * 43758.5453);

    vec3 color = vec3(1.0, 0.92, 0.62) * core;
    color += vec3(1.0, 0.18, 0.0) * beam * 1.5;
    color += vec3(0.9, 0.04, 0.0) * glow;
    color += vec3(1.0, 0.16, 0.0) * sheath * SHEATH_INTENSITY;
    color += vec3(1.0, 0.28, 0.02) * muzzleGlow * MUZZLE_GLOW_INTENSITY;
    color += vec3(1.0, 0.95, 0.65) * muzzleCore * MUZZLE_CORE_INTENSITY;
    color *= scan;
    color += (noise - 0.5) * 0.06;

    float alpha = clamp(core + beam * 0.85 + glow * 0.55 + sheath * SHEATH_ALPHA + muzzleGlow * 0.8 + muzzleCore, 0.0, 1.0);
    gl_FragColor = vec4(color, alpha) * fragColor;
}
