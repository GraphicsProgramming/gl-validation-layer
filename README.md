# OpenGL Validation Layer

## What is this?

If you are familiar with the Vulkan API, you have probably heard of *Validation Layers* before. 
Tey are a system built into the API to check API calls for common mistakes. This library
attempts to do a similar thing, but for OpenGL.

## Why not use `glDebugMessageCallback`?

This is not intended to replace `glDebugMessageCallback`, but to enhance it. Messages reported
by this extension are different across vendors and often not very extensive. We want to provide
a solution that gives clear feedback, tests for more errors and is easy to use.

## Setup

The library exposes a CMake target `gl_debug_layer`. Once your app links to it, simply call
`gl_layer_init()` with the OpenGL version you are using. To register the callback,
`gl_layer_callback()` must be called after every OpenGL call you make. If you are using
the debug version of `GLAD`, this is as easy as calling `glad_set_post_callback(&gl_layer_callback)`.

On exit, call `gl_layer_terminate()` to free resources.

### Diagnostics

Reported error messages may include a link to a file on this repository with more detailed
information. If you wish to see all these files, look in the `diagnostics/` folder.