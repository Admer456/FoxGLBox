# FoxGLBox

A small renderer project for learning.  

## Downloads

None. If you're a non-programmer, you got nothing interesting to look for here.

## Building

Currently this only builds on **x64 Windows**, but it should be buildable on Linux in the near future.

Run CMake and enable `FOX_BUILD_SAMPLES`. Project files for Visual Studio will be generated, from which you can then build this.   
If it fails, submit an issue!

FoxGLBox supports Assimp too, however, you must build it yourself and CMake will expect a specific .lib file (`extern/assimp/lib/assimp-vc142-mt.lib`). This is obviously very early, so you can just use the internal OBJ loader instead.

## Example

So, the idea behind this thing is more or less inspired by idTech 4's renderer:
1. Load a model with its associated materials & textures
2. Create a render entity that uses that model
3. Update the render entity every frame if it's dynamic, or just let it exist in the "render world" if it's a static prop

Step 1:
```cpp
RenderModelParams modelParams {
	"text2.obj", // model path relative to the .exe
	RenderModelType::FromFile
};
RenderModelHandle model = renderWorld->CreateModel( modelParams );
```

Step 2:
```cpp
RenderEntityParams entityParams {
    glm::vec3( 0.0f, 0.0f, 0.0f ), // position
    glm::identity<glm::mat4>(), // orientation
    model // model handle
};
RenderEntityHandle entity = renderWorld->CreateEntity( entityParams );
```
Down the road, you can modify `entityParams` and call `renderWorld->UpdateEntity` to update this render entity's position etc.
```cpp
entityParams.position.z = sin( time ) * 2.0f;
renderWorld->UpdateEntity( entity, entityParams );
```

Other than that, there's the concept of a render view, which is essentially a camera-viewport combo.

# Plans

Lots of stuff! In random order:
- instanced rendering
- more fleshed out material system
- various volumetrics: fog, smoke, clouds
- transparency and potentially order-independent transparency via depth peeling or another, better method
- render target textures (cameras rendering their view to a dynamic texture)
- Z-feather a.k.a. soft particle billboards
- shadowmapping
- environment mapping
- billboards/sprites
- particles (Effekseer integration alternatively?)
- plugin system so I don't have to jam more internal model loaders (imagine a Quake BSP model loading plugin)
- framebuffers for OpenGL backends
- Vulkan backend?
- LODs
