# Urgent

# General
* Add a UI (imgui, nanogui, gwen,  etc)
* Look into the align layout qualifier
* Replace rand() with \<random>
* Add texture support so we can test with transparency
* Test if it would be more performant to to use a vec3 instead of a vec4 for accum in forward/super_sample_frag.glsl

# Maintenance
* Add a README
* Simplify RendererForward constructor

# Render Modes:
- [X] Forward
- [ ] Forward+
- [ ] Defered Rendering
- [ ] Defered Lighting
- [ ] Clustered
- [ ] Tiled
- [ ] Light Indexed Deferred Rendering

# AA Modes
- [ ] SSAA
- [ ] MSAA
- [ ] FXAA
- [ ] MLAA
- [ ] MFAA
- [ ] SMAA
- [ ] TAA
- [ ] TXAA
- [ ] CSAA
- [ ] EQAA

# Reference / Reading
* https://mynameismjp.wordpress.com/2012/10/24/msaa-overview/
* https://mynameismjp.wordpress.com/2010/08/16/deferred-msaa/